/*	SCCS Id: @(#)sounds.c	3.4	2002/05/06	*/
/*	Copyright (c) 1989 Janet Walz, Mike Threepoint */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#ifdef USER_SOUNDS
# ifdef USER_SOUNDS_REGEX
#include <regex.h>
# endif
#endif

#define 	BLESS_CURSES	1
#define 	BLESS_LUCK		2
#define 	BLESS_WEP		3
#define 	UNSTERILIZE		5
#define 	SANCTIFY_WEP	4

#define		NURSE_FULL_HEAL			1
#define		NURSE_TRANQUILIZERS		2
#define		NURSE_RESTORE_ABILITY	3
#define		NURSE_FIX_MORGUL		4
#define		NURSE_FIX_SICKNESS		5
#define		NURSE_FIX_SLIME			6
#define		NURSE_FIX_STERILE		7
#define		NURSE_BRAIN_SURGERY		8

#define		RENDER_FIX_MORGUL		1
#define		RENDER_FIX_SICKNESS		2
#define		RENDER_FIX_SLIME		3
#define		RENDER_BRAIN_SURGERY	4
#define		RENDER_THOUGHT			5

//Match order of constants
const int nurseprices[] = {
	0,	 //0 (invalid)
	2000,//1 10x full healing
	1000,//2 10x sleeping
	1000,//3 10x restore ability
	4000,//4 20x full healing
	2000,//5 same as healing
	2000,//6 same as healing
	1000,//7 restore ability
	6000,//8 20x trephination kit cost
};

#ifdef OVLB

static const char *FDECL(DantalionRace,(int));
int FDECL(dobinding,(int, int));
int * FDECL(spirit_skills, (int));
static int FDECL(doyochlolmenu, (struct monst *));
static int NDECL(doblessmenu);
static int NDECL(donursemenu);
static int NDECL(dorendermenu);
static int FDECL(dodollmenu, (struct monst *));
static boolean FDECL(nurse_services,(struct monst *));
static boolean FDECL(render_services,(struct monst *));
static boolean FDECL(buy_dolls,(struct monst *));

static const char tools[] = { TOOL_CLASS, 0 };

#endif /* OVLB */

#ifdef OVL0

static int FDECL(mon_in_room, (struct monst *,int));

//I am Buer, X,
static const char *buerTitles[] = {
	"grandmother huntress",
	"grandfather hunter",
	"the wandering sage",
	"the forsaken sage",
	"the banished sage",
	"itinerant teacher",
	"fallen of heaven",
	"risen of hell",
	"from beyond the fixed stars",
	"the first healer",
	"philosopher of nature",
	"of the philosophy of healing",
	"penitent assassin",
	"philosopher of healing",
	"philosopher of natural morality",
	"the lost",
	"the ignored",
	"of the east",
	"of the west",
	"of the north",
	"of the south",
	"daughter of the Moon",
	"son of the Moon",
	"paragon of morality"
};
//I am Buer, X,
static const char *buerSetOne[] = {
	"cursed by a witch",
	"doomed by the gods",
	"seduced by a demon and made",
	"taken by a fey spirit and left",
	"come from a far place",
	"descended",
	"ascended",
	"destined",
	"determined"
};
//to
static const char *buerSetTwo[] = {
	"wander for all time",
	"walk through the ages",
	"search the five corners of the world",
	"bear witness 'til all is redemed at last",
	"live forever on this winding road",
	"never be released by death",
	"never find home",
	"search for redemption, but never to find it",
	"seek virtue forevermore"
};

static const char *echidnaTitles[] = {
	"the She Viper",
	"mate of Typhon",
	"daughter of Tartarus and Gaia",
	"daughter of hell and earth",
	"enemy of the Gods",
	"grim and ageless",
	"guardian of Arima",
	"Mother of Monsters"
};

static const char *alignmentThings[] = {
	"Can a paladin kill baby orcs?",
	"A paladin must kill baby orcs?",
	"Must a paladin never stab a man in the back?",
	"Must laws be upheld with no reason or logic?",
	"Saying you love someone is always good?",
	"Can a king be lawful?",
	"Can God be lawful?",
	"Is it chaotic to refuse to kill an innocent man?",
	"Chaotic means stabbing a man, then giving him ice cream!",
	"Neutral means stabbing a man, then giving him ice cream!",
	"Are you compelled to do evil, regardless of its utility?",
	"Being nailed to things is good?",
	"Storms are chaotic?",
	"Armies are lawful?",
	"Must all rules be followed?",
	"Everything not forbidden is compulsory?"
};

static const char *woePrisoners[] = {
	"All this, because of a dream?",
	"Why are they doing this?",
	"I thought these religions had nothing to do with each other!?",
	"Shouldn't these gods be enemies?",
	"To what Power are the Keter sworn?",
	"They said the world was cracked. They said it was our fault.",
	"They said there is an Enemy in the Void.",
	"They say the Enemy seeks to destroy the world, though she is not of it.",
	"I heard them talking about a device to force open a path to the Void.",
	"I saw them bring in a silver bell.",
	"I heard a bell ring frantically, just a short while ago."
};

static const char *embracedPrisoners[] = {
	"Lolth, help me!",
	"I can't control my arms!",
	"I can't stop!  Look out!!",
	"Cut me loose, please!",
	"It's so dark!",
	"Free me!",
	"Lolth, why have you forsaken me!?",
	"It was waiting for us!",
	"Kill me!  Please, just kill me...",
	"Why?"
};

static const char *embracedAlider[] = {
	"Mother, help me!",
	"I can't control my arms!",
	"I can't stop!  Look out!!",
	"Cut me loose, please!",
	"It's so dark!",
	"Free me!",
	"Kill me!  Please, just kill me...",
	"Why?"
};

static const char *freedAlider[] = {
	"We fought valiantly, but we were overrun by the forces of Ilsensine.",
	"I have seen the flowers at the end of time.",
	"Ilsensine controls the future, the whole universe is her body.",
	"The Hero must have failed. We must use the Annulus against Ilsensine.",
	"We must seek hope in the past. There is none left in the fading corpse of the future.",
	"Ilsensine must be excised from the past.",
	"Even the gods could not defeat Ilsensine. They failed and were consumed.",
	"...I still hear the screams.",
	"...Mother, Father, will I see you again?",
	"Divine Mother, I am coming."
};

static const char *agonePrisoner[] = {
	"Who am I?",
	"Where am I?",
	"What are you doing?",
	"Do I know you?",
	"I've forgotten something.",
	"Sub- NO!",
	"...Lolth. Lolth, help me!",
	"...I can't control my body.",
	"It's watching me..."
};

static const char *thrallPrisoners[] = {
	"Submit.",
	"Give in.",
	"Rest.",
	"Give up.",
	"(Why?)",
	"You're suffering.",
	"Penumbra hangs over all.",
	"It's easier this way."
};

static const char *parasitizedDroid[] = {
	">Motor Cortex Compromised<",
	">Cover Opened, Warranty Void<",
	">System Error<",
	"Run!",
	"I can't control my limbs!",
	"I can't stop!  Look out!!",
	"Free me!",
	"You must destroy me!",
	"Why!?"
};

/* this easily could be a macro, but it might overtax dumb compilers */
static int
mon_in_room(mon, rmtyp)
struct monst *mon;
int rmtyp;
{
    int rno = levl[mon->mx][mon->my].roomno;

    return rno >= ROOMOFFSET && rooms[rno - ROOMOFFSET].rtype == rmtyp;
}

void
dosounds()
{
    register struct mkroom *sroom;
    register int hallu, vx, vy;
#if defined(AMIGA) && defined(AZTEC_C_WORKAROUND)
    int xx;
#endif
    struct monst *mtmp;

    if (!flags.soundok || u.uswallow || Underwater) return;

    hallu = Hallucination ? 1 : 0;

    if (level.flags.nfountains && !rn2(400)) {
	static const char * const fountain_msg[4] = {
		"bubbling water.",
		"water falling on coins.",
		"the splashing of a naiad.",
		"a soda fountain!",
	};
	You_hear1(fountain_msg[rn2(3)+hallu]);
    }
	if (level.flags.nforges && !rn2(300)) {
		static const char *const forge_msg[3] = {
			"a slow bubbling.", "crackling flames.", "chestnuts roasting on an open fire.",
		};
		You_hear1(forge_msg[rn2(2) + hallu]);
	}
#ifdef SINK
    if (level.flags.nsinks && !rn2(300)) {
	static const char * const sink_msg[3] = {
		"a slow drip.",
		"a gurgling noise.",
		"dishes being washed!",
	};
	You_hear(sink_msg[rn2(2)+hallu]);
    }
#endif
    if (level.flags.has_court && !rn2(200)) {
	static const char * const throne_msg[4] = {
		"the tones of courtly conversation.",
		"a sceptre pounded in judgment.",
		"Someone shouts \"Off with %s head!\"",
		"Queen Beruthiel's cats!",
	};
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if ((mtmp->msleeping ||
			is_lord(mtmp->data) || is_prince(mtmp->data)) &&
		!is_animal(mtmp->data) &&
		mon_in_room(mtmp, COURT)) {
		/* finding one is enough, at least for now */
		int which = rn2(3)+hallu;

		if (which != 2) You_hear1(throne_msg[which]);
		else		pline(throne_msg[2], uhis());
		return;
	    }
	}
    }
    if (level.flags.has_garden && !rn2(200)) {
	static const char *garden_msg[4] = {
		"crickets chirping!",
		"birds singing!",
		"grass growing!",
		"wind in the willows!",
	};
	You_hear("%s", garden_msg[rn2(2) + 2 * hallu]);
	return;
    }
    if (level.flags.has_library && !rn2(200)) {
	static const char *library_msg[4] = {
		"dripping water.",
		"pages turning.",
		"audible silence.",
		"a librarian yelling at someone to be SILENT!",
	};
	You_hear("%s", library_msg[rn2(2) + 2 * hallu]);
	return;
    }
    if (level.flags.has_armory && !rn2(200)) {
	static const char *armory_msg[4] = {
		"water dripping onto metal.",
		"a ceiling tile fall.",
		"iron oxidize.",
		"a military contractor making out like a bandit.",
	};
	You_hear("%s", armory_msg[rn2(2) + 2 * hallu]);
	return;
    }
    if (level.flags.has_swamp && !rn2(200)) {
	static const char * const swamp_msg[3] = {
		"hear mosquitoes!",
		"smell marsh gas!",	/* so it's a smell...*/
		"hear Donald Duck!",
	};
	You1(swamp_msg[rn2(2)+hallu]);
	return;
    }
    if (level.flags.has_vault && !rn2(200)) {
	if (!(sroom = search_special(VAULT))) {
	    /* strange ... */
	    level.flags.has_vault = 0;
	    return;
	}
	if(gd_sound())
	    switch (rn2(2)+hallu) {
		case 1: {
		    boolean gold_in_vault = FALSE;

		    for (vx = sroom->lx;vx <= sroom->hx; vx++)
			for (vy = sroom->ly; vy <= sroom->hy; vy++)
			    if (g_at(vx, vy))
				gold_in_vault = TRUE;
#if defined(AMIGA) && defined(AZTEC_C_WORKAROUND)
		    /* Bug in aztec assembler here. Workaround below */
		    xx = ROOM_INDEX(sroom) + ROOMOFFSET;
		    xx = (xx != vault_occupied(u.urooms));
		    if(xx)
#else
		    if (vault_occupied(u.urooms) !=
			 (ROOM_INDEX(sroom) + ROOMOFFSET))
#endif /* AZTEC_C_WORKAROUND */
		    {
			if (gold_in_vault)
			    You_hear(!hallu ? "someone counting money." :
				"the quarterback calling the play.");
			else
			    You_hear("someone searching.");
			break;
		    }
		    /* fall into... (yes, even for hallucination) */
		}
		case 0:
		    You_hear("the footsteps of a guard on patrol.");
		    break;
		case 2:
		    You_hear("Ebenezer Scrooge!");
		    break;
	    }
	return;
    }
    if (level.flags.has_beehive && !rn2(200)) {
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if ((mtmp->data->mlet == S_ANT && mon_resistance(mtmp,FLYING)) &&
		mon_in_room(mtmp, BEEHIVE)) {
		switch (rn2(2)+hallu) {
		    case 0:
			You_hear("a low buzzing.");
			break;
		    case 1:
			You_hear("an angry drone.");
			break;
		    case 2:
			You_hear("bees in your %sbonnet!",
			    uarmh ? "" : "(nonexistent) ");
			break;
		}
		return;
	    }
	}
    }
    if (level.flags.has_morgue && !rn2(200)) {
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if (is_undead(mtmp->data) &&
		mon_in_room(mtmp, MORGUE)) {
		switch (rn2(2)+hallu) {
		    case 0:
			You("suddenly realize it is unnaturally quiet.");
			break;
		    case 1:
			pline_The("%s on the back of your %s stands up.",
				body_part(HAIR), body_part(NECK));
			break;
		    case 2:
			pline_The("%s on your %s seems to stand up.",
				body_part(HAIR), body_part(HEAD));
			break;
		}
		return;
	    }
	}
    }
    if (level.flags.has_barracks && !rn2(200)) {
	static const char * const barracks_msg[4] = {
		"blades being honed.",
		"loud snoring.",
		"dice being thrown.",
		"General MacArthur!",
	};
	int count = 0;

	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if (is_mercenary(mtmp->data) &&
#if 0		/* don't bother excluding these */
		!strstri(mtmp->data->mname, "watch") &&
		!strstri(mtmp->data->mname, "guard") &&
#endif
		mon_in_room(mtmp, BARRACKS) &&
		/* sleeping implies not-yet-disturbed (usually) */
		(mtmp->msleeping || ++count > 5)) {
		You_hear1(barracks_msg[rn2(3)+hallu]);
		return;
	    }
	}
    }
	if (level.flags.has_island && !rn2(200)) {
		static const char *island_msg[4] = {
			"hear seagulls.",
			"hear waves on sand.",
			"hear burly voices singing shanties.",
			"here someone ask about warez.",
		};
		You1(island_msg[rn2(2)+2*hallu]);
		return;
	}
    if (level.flags.has_zoo && !rn2(200)) {
	static const char * const zoo_msg[3] = {
		"a sound reminiscent of an elephant stepping on a peanut.",
		"a sound reminiscent of a seal barking.",
		"Doctor Doolittle!",
	};
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if ((mtmp->msleeping || is_animal(mtmp->data)) &&
		    mon_in_room(mtmp, ZOO)) {
		You_hear1(zoo_msg[rn2(2)+hallu]);
		return;
	    }
	}
    }
    if (level.flags.has_shop && !rn2(200)) {
	if (!(sroom = search_special(ANY_SHOP))) {
	    /* strange... */
	    level.flags.has_shop = 0;
	    return;
	}
	if (tended_shop(sroom) &&
		!index(u.ushops, ROOM_INDEX(sroom) + ROOMOFFSET)) {
	    static const char * const shop_msg[3] = {
		    "someone cursing shoplifters.",
		    "the chime of a cash register.",
		    "Neiman and Marcus arguing!",
	    };
	    You_hear1(shop_msg[rn2(2)+hallu]);
	}
	return;
    }
    if (Is_oracle_level(&u.uz) && !rn2(400)) {
	/* make sure the Oracle is still here */
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
	    if (!DEADMONSTER(mtmp) && mtmp->mtyp == PM_ORACLE)
		break;
	/* and don't produce silly effects when she's clearly visible */
	if (mtmp && (hallu || !canseemon(mtmp))) {
	    static const char * const ora_msg[5] = {
		    "a strange wind.",		/* Jupiter at Dodona */
		    "convulsive ravings.",	/* Apollo at Delphi */
		    "snoring snakes.",		/* AEsculapius at Epidaurus */
		    "someone say \"No more woodchucks!\"",
		    "a loud ZOT!"		/* both rec.humor.oracle */
	    };
		int messagen;
		messagen = rn2(3)+hallu*2;
	    You_hear1(ora_msg[messagen]);
		if(messagen == 3){
			makemon(&mons[PM_WOODCHUCK], 0, 0,MM_ADJACENTOK|NO_MINVENT);
		} else if(messagen == 4){
			struct monst *tmpm;
			for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
				if(tmpm->mtyp == PM_WOODCHUCK){
					if (resists_death(tmpm)) {
						// if (canseemon(tmpm))
							// pline("%s seems no deader than before.", Monnam(tmpm));
					} else if (!(resists_magm(tmpm) || resist(tmpm, 0, 0, FALSE))) {
							tmpm->mhp = -1;
						monkilled(tmpm, "", AD_SPEL);
			break;
					}
				}
			}
			if(!tmpm){ /*pointer is stale, but still nonzero*/
				if(youracedata->mtyp == PM_WOODCHUCK){
					You("have an out of body experience."); //You are hallucinating if you got this message
				}
			}
		}
	}
	return;
    }
}

#endif /* OVL0 */
#ifdef OVLB

static const char * const h_sounds[] = {
    "beep", "boing", "sing", "belche", "creak", "cough", "rattle",
    "ululate", "pop", "jingle", "sniffle", "tinkle", "eep",
    "clatter", "hum", "sizzle", "twitter", "wheeze", "rustle",
    "honk", "lisp", "yodel", "coo", "burp", "moo", "boom",
    "murmur", "oink", "quack", "rumble", "twang", "bellow",
    "toot", "gargle", "hoot", "warble"
};

const char *
growl_sound(mtmp)
register struct monst *mtmp;
{
	const char *ret;

	switch (is_silent_mon(mtmp) ? MS_SILENT : 
			mtmp->ispriest ? MS_PRIEST : 
			mtmp->isshk ? MS_SELL : 
			(mtmp->data->msound == MS_GLYPHS) ? MS_SILENT : 
			mtmp->data->msound
	) {
	case MS_MEW:
	case MS_HISS:
	case MS_APOC:
	    ret = "hiss";
	    break;
	case MS_BARK:
	case MS_GROWL:
	    ret = "growl";
	    break;
	case MS_SHEEP:
		ret = "snort";
		break;
	case MS_ROAR:
	    ret = "roar";
	    break;
	case MS_BUZZ:
	    ret = "buzz";
	    break;
	case MS_SQEEK:
	    ret = "squeal";
	    break;
	case MS_SQAWK:
	    ret = "screech";
	    break;
	case MS_NEIGH:
	    ret = "neigh";
	    break;
	case MS_WAIL:
	    ret = "wail";
	    break;
	case MS_RIBBIT:
	    ret = "croak";
	    break;
	case MS_SILENT:
		ret = "commotion";
		break;
	default:
		ret = "scream";
	}
	return ret;
}

/* the sounds of a seriously abused pet, including player attacking it */
void
growl(mtmp)
register struct monst *mtmp;
{
    register const char *growl_verb = 0;

    if (mtmp->msleeping || !mtmp->mcanmove || !mtmp->data->msound)
	return;

    /* presumably nearness and soundok checks have already been made */
    if (Hallucination)
	growl_verb = h_sounds[rn2(SIZE(h_sounds))];
    else
	growl_verb = growl_sound(mtmp);
    if (growl_verb) {
	pline("%s %s!", Monnam(mtmp), vtense((char *)0, growl_verb));
	if(flags.run) nomul(0, NULL);
	wake_nearto_noisy(mtmp->mx, mtmp->my, mtmp->data->mlevel * 18);
    }
}

/* the sounds of mistreated pets */
void
yelp(mtmp)
struct monst *mtmp;
{
    const char *yelp_verb = 0;
	const char *yelp_modifier = 0;

    if (mtmp->msleeping || !mtmp->mcanmove || !mtmp->mnotlaugh || !mtmp->data->msound)
	return;

    /* presumably nearness and soundok checks have already been made */
    if (Hallucination)
	yelp_verb = h_sounds[rn2(SIZE(h_sounds))];
    else switch (mtmp->data->msound) {
	case MS_MEW:
	    yelp_verb = "yowl";
	    break;
	case MS_SHEEP:
		yelp_verb = "bleat";
		break;
	case MS_BARK:
	case MS_GROWL:
	    yelp_verb = "yelp";
	    break;
	case MS_ROAR:
	    yelp_verb = "snarl";
	    break;
	case MS_SQEEK:
	    yelp_verb = "squeal";
	    break;
	case MS_SQAWK:
	    yelp_verb = "screak";
	    break;
	case MS_WAIL:
	    yelp_verb = "wail";
	    break;
	case MS_RIBBIT:
	    yelp_verb = "peep";
	    break;
	case MS_COUGH:
	    yelp_verb = "cough";
		yelp_modifier = " paroxysmally";
	    break;
    }
    if (yelp_verb) {
	pline("%s %s%s!", Monnam(mtmp), vtense((char *)0, yelp_verb), yelp_modifier ? yelp_modifier : "");
	if(flags.run) nomul(0, NULL);
	wake_nearto_noisy(mtmp->mx, mtmp->my, mtmp->data->mlevel * 12);
    }
}

/* the sounds of distressed pets */
void
whimper(mtmp)
register struct monst *mtmp;
{
    register const char *whimper_verb = 0;

    if (mtmp->msleeping || !mtmp->mcanmove || !mtmp->mnotlaugh || !mtmp->data->msound)
	return;

    /* presumably nearness and soundok checks have already been made */
    if (Hallucination)
	whimper_verb = h_sounds[rn2(SIZE(h_sounds))];
    else switch (mtmp->data->msound) {
	case MS_MEW:
	case MS_GROWL:
	    whimper_verb = "whimper";
	    break;
	case MS_SHEEP:
		whimper_verb = "bleat";
		break;
	case MS_BARK:
	    whimper_verb = "whine";
	    break;
	case MS_SQEEK:
	    whimper_verb = "squeal";
	    break;
    }
    if (whimper_verb) {
	pline("%s %s.", Monnam(mtmp), vtense((char *)0, whimper_verb));
	if(flags.run) nomul(0, NULL);
	wake_nearto(mtmp->mx, mtmp->my, mtmp->data->mlevel * 6);
    }
}

/* pet makes "I'm hungry" noises */
void
beg(mtmp)
register struct monst *mtmp;
{
    if (mtmp->msleeping || !mtmp->mcanmove || !mtmp->mnotlaugh ||
	    !(carnivorous(mtmp->data) || herbivorous(mtmp->data)))
	return;

    /* presumably nearness and soundok checks have already been made */
    if (!is_silent_mon(mtmp) && mtmp->data->msound <= MS_ANIMAL)
	(void) domonnoise(mtmp, TRUE);
    else if (mtmp->data->msound >= MS_HUMANOID) {
	if (!canspotmon(mtmp))
	    map_invisible(mtmp->mx, mtmp->my);
	verbalize("I'm hungry.");
    }
}

int
domonnoise(mtmp, chatting)
struct monst *mtmp;
boolean chatting;
{
	register const char *pline_msg = 0,	/* Monnam(mtmp) will be prepended */
			*verbl_msg = 0;	/* verbalize() */
	struct permonst *ptr = mtmp->data;
	char verbuf[BUFSZ];
	char msgbuff[BUFSZ];
	char class_list[MAXOCLASSES+2];

	if(mtmp && noactions(mtmp)){
		if(mtmp->mtrapped && t_at(mtmp->mx, mtmp->my) && t_at(mtmp->mx, mtmp->my)->ttyp == VIVI_TRAP){
			if(chatting && canspotmon(mtmp))
				pline("%s is sleeping peacefully; presumably the doing of the delicate equipment that displays %s vivisected form.", 
					Monnam(mtmp), (is_animal(mtmp->data) || mindless_mon(mtmp) ? "its" : hisherits(mtmp))
				);
		}
		else if(mtmp->entangled_otyp == SHACKLES){
			if(chatting && canspotmon(mtmp))
				pline("%s is unconscious.",  Monnam(mtmp));
		}
		else {
			if(chatting && canspotmon(mtmp))
				pline("%s struggles against %s bindings.", 
					Monnam(mtmp), (is_animal(mtmp->data) || mindless_mon(mtmp) ? "its" : hisherits(mtmp))
				);
		}
		return 0;
	}
	
	/* Make sure its your role's quest quardian; adjust if not */
	if (ptr->msound == MS_GUARDIAN && ptr->mtyp != urole.guardnum && ptr->mtyp != PM_CELEBORN){
		int mndx = monsndx(ptr);
		ptr = &mons[genus(mndx,1)];
	}

	/* be sure to do this before talking; the monster might teleport away, in
	 * which case we want to check its pre-teleport position
	 */
	if (!canspotmon(mtmp) && distmin(u.ux,u.uy,mtmp->mx,mtmp->my) < 2 && ptr->msound != MS_SONG && 
		ptr->msound != MS_INTONE && ptr->msound != MS_FLOWER && ptr->msound != MS_OONA
	) map_invisible(mtmp->mx, mtmp->my);
	mtmp->mnoise = TRUE;
	
	if(mtmp->mtame && is_yochlol(mtmp->data) && yn("(Ask to change form?)") == 'y'){
		int pm = doyochlolmenu(mtmp);
		if(pm){
			were_transform(mtmp, pm);
			return 1;
		}
	}
	
    /* presumably nearness and sleep checks have already been made */
	if (!flags.soundok) return(0);
	if (is_silent_mon(mtmp)){
		if (chatting) {
			pline("%s does not respond.", Monnam(mtmp));
			return 1;
		}
		return(0);
	}
	
	if(mtmp->ispriest){
		priest_talk(mtmp);
		return 1;
	}
	
	if (mtmp->mtame && !flags.mon_moving && uclockwork && !nohands(ptr) && !is_animal(ptr) && yn("(Ask for help winding your clockwork?)") == 'y'){
		struct obj *key;
		int turns = 0;
		
		Strcpy(class_list, tools);
		key = getobj(class_list, "wind with");
		if (!key){
			pline1(Never_mind);
		} else {
			turns = ask_turns(mtmp, 0, 0);
			if(!turns){
				pline1(Never_mind);
			} else {
				start_clockwinding(key, mtmp, turns);
				return 1;
			}
		}
	}
	int soundtype = ptr->msound;
	//Faction and special abilities adjustment
	if(mtmp->mtyp == PM_RHYMER && !mtmp->mspec_used)
		soundtype = MS_SONG;
	else if(mtmp->mfaction == QUEST_FACTION && mtmp->mtyp != PM_SIR_ALJANOR)
		soundtype = MS_GUARDIAN;
	else if(mtmp->mtyp == urole.guardnum)
		soundtype = MS_GUARDIAN;
	else if(ptr->msound == MS_CUSS && mtmp->mpeaceful)
		soundtype = MS_HUMANOID;

	//Don't sing if chatted to.
	if(chatting && (soundtype == MS_SONG || soundtype == MS_OONA)){
		if(mtmp->mfaction == QUEST_FACTION)
			soundtype = MS_GUARDIAN;
		if(mtmp->mtyp == urole.guardnum)
			soundtype = MS_GUARDIAN;
		else soundtype = MS_HUMANOID;
	}

	//Template and profession adjustments
	if(is_silent_mon(mtmp))
		soundtype = MS_SILENT;
	else if(is_dollable(mtmp->data) && mtmp->m_insight_level)
		soundtype = MS_STATS;
	else if(mtmp->ispriest)
		soundtype = MS_PRIEST;
	else if(mtmp->isshk)
		soundtype = MS_SELL;

	switch (soundtype) {
	case MS_ORACLE:
	    return doconsult(mtmp);
	case MS_PRIEST: /*Most (all?) things with this will have ispriest set*/
	    priest_talk(mtmp);
	    break;
	case MS_NEMESIS:
	  if(Race_if(PM_DROW) && !Role_if(PM_NOBLEMAN) && !flags.stag && (yn("Betray your current quest leader and join the uprising?") == 'y')){
		turn_stag();
	break;
	  }
	case MS_LEADER:
	case MS_GUARDIAN:
asGuardian:
	    if (mtmp->mpeaceful && uclockwork && !mtmp->mtame && !nohands(ptr) && !is_animal(ptr) && yn("(Ask for help winding your clockwork?)") == 'y'){
			struct obj *key;
			int turns = 0;
			
			Strcpy(class_list, tools);
			key = getobj(class_list, "wind with");
			if (!key){
				pline1(Never_mind);
				break;
			}
			turns = ask_turns(mtmp, 0, 0);
			if(!turns){
				pline1(Never_mind);
				break;
			}
			start_clockwinding(key, mtmp, turns);
			break;
		}
	    quest_chat(mtmp);
	    break;
	case MS_SELL: /* pitch, pay, total */
	    shk_chat(mtmp);
	    break;
	case MS_VAMPIRE:
	    {
	    /* vampire messages are varied by tameness, peacefulness, and time of night */
		boolean isnight = night();
		boolean kindred = is_vampire(youracedata);
		boolean nightchild = (Upolyd && (u.umonnum == PM_WOLF ||
				       u.umonnum == PM_WINTER_WOLF ||
	    			       u.umonnum == PM_WINTER_WOLF_CUB));
		const char *racenoun = (flags.female && urace.individual.f) ?
					urace.individual.f : (urace.individual.m) ?
					urace.individual.m : urace.noun;

		if (mtmp->mtame) {
			if (kindred) {
				Sprintf(verbuf, "Good %s to you Master%s",
					isnight ? "evening" : "day",
					isnight ? "!" : ".  Why do we not rest?");
				verbl_msg = verbuf;
		    	} else {
		    	    Sprintf(verbuf,"%s%s",
				nightchild ? "Child of the night, " : "",
				midnight() ?
					"I can stand this craving no longer!" :
				isnight ?
					"I beg you, help me satisfy this growing craving!" :
					"I find myself growing a little weary.");
				verbl_msg = verbuf;
			}
		} else if (mtmp->mpeaceful) {
			if (kindred && isnight) {
				Sprintf(verbuf, "Good feeding %s!",
	    				flags.female ? "sister" : "brother");
				verbl_msg = verbuf;
 			} else if (nightchild && isnight) {
				Sprintf(verbuf,
				    "How nice to hear you, child of the night!");
				verbl_msg = verbuf;
	    		} else
		    		verbl_msg = "I only drink... potions.";
    	} else {
			int vampindex;
	    		static const char * const vampmsg[] = {
			       /* These first two (0 and 1) are specially handled below */
	    			"I vant to suck your %s!",
	    			"I vill come after %s without regret!",
		    	       /* other famous vampire quotes can follow here if desired */
	    		};
			if (kindred)
			    verbl_msg = "This is my hunting ground that you dare to prowl!";
			else if (youracedata->mtyp == PM_SILVER_DRAGON ||
				 youracedata->mtyp == PM_BABY_SILVER_DRAGON) {
			    /* Silver dragons are silver in color, not made of silver */
			    Sprintf(verbuf, "%s! Your silver sheen does not frighten me!",
					youracedata->mtyp == PM_SILVER_DRAGON ?
					"Fool" : "Young Fool");
			    verbl_msg = verbuf; 
			} else {
			    vampindex = rn2(SIZE(vampmsg));
			    if (vampindex == 0) {
				Sprintf(verbuf, vampmsg[vampindex], body_part(BLOOD));
	    			verbl_msg = verbuf;
			    } else if (vampindex == 1) {
				Sprintf(verbuf, vampmsg[vampindex],
					Upolyd ? an(mons[u.umonnum].mname) : an(racenoun));
	    			verbl_msg = verbuf;
		    	    } else
			    	verbl_msg = vampmsg[vampindex];
			}
	    }
	}
	break;
	case MS_WERE:
		if (flags.moonphase == FULL_MOON && (night() ^ !rn2(13))) {
			pline("%s throws back %s head and lets out a blood curdling %s!",
				  Monnam(mtmp), mhis(mtmp),
				  ptr->mtyp == PM_HUMAN_WERERAT ? "shriek" : "howl");
			wake_nearto_noisy(mtmp->mx, mtmp->my, 11*11);
		} else
			pline_msg =
				 "whispers inaudibly.  All you can make out is \"moon\".";
	break;
	case MS_BARK:
	    if (flags.moonphase == FULL_MOON && night()) {
		pline_msg = "howls.";
	    } else if (mtmp->mpeaceful) {
		if (mtmp->mtame &&
			(mtmp->mconf || mtmp->mflee || mtmp->mtrapped ||
			 moves > EDOG(mtmp)->hungrytime || mtmp->mtame < 5))
		    pline_msg = "whines.";
		else if (mtmp->mtame && EDOG(mtmp)->hungrytime > moves + 1000)
		    pline_msg = "yips.";
		else {
		    if (mtmp->mtyp != PM_DINGO)	/* dingos do not actually bark */
			    pline_msg = "barks.";
		}
	    } else {
		pline_msg = "growls.";
	    }
	    break;
	case MS_SHEEP:
		pline_msg = "baaaas.";
		break;
	case MS_MEW:
	    if (mtmp->mtame) {
		if (mtmp->mconf || mtmp->mflee || mtmp->mtrapped ||
			mtmp->mtame < 5)
		    pline_msg = "yowls.";
		else if (moves > EDOG(mtmp)->hungrytime)
		    pline_msg = "meows.";
		else if (EDOG(mtmp)->hungrytime > moves + 1000)
		    pline_msg = "purrs.";
		else
		    pline_msg = "mews.";
		break;
	    } /* else FALLTHRU */
	case MS_GROWL:
	    pline_msg = mtmp->mpeaceful ? "snarls." : "growls!";
	    break;
	case MS_ROAR:
	    pline_msg = mtmp->mpeaceful ? "snarls." : "roars!";
	    break;
	case MS_SQEEK:
	    pline_msg = "squeaks.";
	    break;
	case MS_SQAWK:
	    if (ptr->mtyp == PM_RAVEN && !mtmp->mpeaceful)
	    	verbl_msg = "Nevermore!";
	    else
	    	pline_msg = "squawks.";
	    break;
	case MS_HISS:
	    if (!mtmp->mpeaceful) pline_msg = "hisses!";
	    else {
			if (chatting) pline_msg = "does not respond.";
			else return 0;	/* no sound */
		}
	    break;
	case MS_BUZZ:
	    pline_msg = mtmp->mpeaceful ? "drones." : "buzzes angrily.";
	    break;
	case MS_GRUNT:
	    pline_msg = "grunts.";
	    break;
	case MS_NEIGH:
	    if (mtmp->mtame < 5)
		pline_msg = "neighs.";
	    else if (moves > EDOG(mtmp)->hungrytime)
		pline_msg = "whinnies.";
	    else
		pline_msg = "whickers.";
	    break;
	case MS_WAIL:
	    pline_msg = "wails mournfully.";
	    break;
	case MS_GURGLE:
	    pline_msg = "gurgles.";
	    break;
	case MS_RIBBIT:
	    pline_msg = mtmp->mpeaceful ? "ribbits." : "croaks.";
	    break;
	case MS_BURBLE:
	    pline_msg = "burbles.";
	    break;
	case MS_COUGH:
		if (mtmp->mflee)
		    pline_msg = "coughs frantically.";
		else if ((get_mx(mtmp, MX_EDOG) && moves > EDOG(mtmp)->hungrytime) || mtmp->mhp*10 < mtmp->mhpmax)
		    pline_msg = "coughs weakly.";
		else switch(rn2(10)){
			default:
				pline_msg = "coughs spasmodically.";
			break;
			case 0:
				pline_msg = "coughs wetly.";
			break;
			case 1:
				pline_msg = "groans weakly.";
			break;
			case 2:
				pline_msg = "mumbles incoherently.";
			break;
			case 3:
				pline_msg = "coughs.";
			break;
			case 4:
				pline_msg = "coughs dryly.";
			break;
			case 5:
				pline_msg = "coughs up blood.";
			break;
			case 6:
				pline_msg = "vomits.";
			break;
		}
	    break;
	case MS_JUBJUB:{
		struct monst *tmpm;
		if(!(mtmp->mspec_used || mtmp->mcan)){
			pline("%s screams high and shrill.", Monnam(mtmp));
			mtmp->mspec_used = 10;
			for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
				if(tmpm != mtmp && !DEADMONSTER(tmpm) && distmin(tmpm->mx, tmpm->my, mtmp->mx, mtmp->my) <= BOLT_LIM){
					if(tmpm->mtame && tmpm->mtame<20) tmpm->mtame++;
					if(d(1,tmpm->mhp) < mtmp->mhpmax){
						tmpm->mflee = 1;
					}
				}
			}
			if(!mtmp->mpeaceful && distmin(u.ux, u.uy, mtmp->mx, mtmp->my) <= BOLT_LIM)
				make_stunned(HStun + mtmp->mhp/10, TRUE);
		}
	}break;
	case MS_TRUMPET:{
		struct obj *otmp;
		struct monst *tmpm;
		otmp = MON_WEP(mtmp);
		if(!(mtmp->mspec_used || mtmp->mcan) && otmp){
			if(canspotmon(mtmp)) pline("%s raises %s %s to %s lips and it becomes a trumpet.", Monnam(mtmp), hisherits(mtmp), xname(otmp), hisherits(mtmp));
			pline("%s blows %s trumpet.", Monnam(mtmp), hisherits(mtmp));
			mtmp->mspec_used = 7;
			for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
				if(tmpm != mtmp && !DEADMONSTER(tmpm)){
					if(tmpm->mpeaceful != mtmp->mpeaceful && !resist(tmpm, 0, 0, FALSE)){
						tmpm->mconf = 1;
					}
				}
			}
			if(!mtmp->mpeaceful && mtmp->mux != 0){
				make_stunned(HStun + mtmp->mhp/10, TRUE);
				struct attack fakesummonspell = { AT_MAGC, AD_CLRC, 0, 6 };
				cast_spell(mtmp, (struct monst *)0, &fakesummonspell, !rn2(4) ? SUMMON_ANGEL : SUMMON_MONS, 0, 0);
			}
			if(uwep && uwep->oartifact == ART_SINGING_SWORD){
				uwep->ovar1_heard |= OHEARD_RALLY;
			}
		}
	}break;
	case MS_HARROW:{
		int i;
		struct monst *tmpm;
		if(!(mtmp->mspec_used || mtmp->mcan) && !mtmp->mpeaceful){
			pline("%s screams.", Monnam(mtmp));
			mtmp->mspec_used = 7;
			for(i = d(2,3); i; i--){
				tmpm = makemon(&mons[rn2(2) ? PM_ZARIELITE_ZEALOT : PM_ZARIELITE_HERETIC], mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_NOCOUNTBIRTH|MM_ESUM);
				if(tmpm){
					mark_mon_as_summoned(tmpm, mtmp, 21, 0);
				}
			}
			if(uwep && uwep->oartifact == ART_SINGING_SWORD){
				uwep->ovar1_heard |= OHEARD_RALLY;
			}
		}
	}break;
	case MS_DREAD:{
		struct monst *tmpm;
		int ix, iy;
		if(mtmp->mvar_dreadPrayer_cooldown < moves && !mtmp->mdoubt && (
			mtmp->mhp < mtmp->mhpmax/4 || mtmp->mcrazed
		)){
			mtmp->mvar_dreadPrayer_cooldown = moves + rnz(350);
			mtmp->mvar_dreadPrayer_progress = moves + 5;
		}
		if(mtmp->mvar_dreadPrayer_progress){
			if(mtmp->mvar_dreadPrayer_progress < moves){
				mtmp->mvar_dreadPrayer_progress = 0;
				mtmp->mhp = mtmp->mhpmax;
				mtmp->mspec_used = 0;
				set_mcan(mtmp, FALSE);
				mtmp->mflee = 0; mtmp->mfleetim = 0;
				mtmp->mcrazed = 0; mtmp->mberserk = 0; mtmp->mdisrobe = 0;
				mtmp->mcansee = 1; mtmp->mblinded = 0;
				mtmp->mcanmove = 1; mtmp->mfrozen = 0;
				mtmp->mnotlaugh = 1; mtmp->mlaughing = 0;
				mtmp->msleeping = 0;
				mtmp->mstun = 0; mtmp->mconf = 0;
				untame(mtmp, 0);
				
				u.ustdy = mtmp->m_lev;
				pline_msg = "ends its prayer.";
			} else {
				pline_msg = "prays, and the whispers fill the world.";
			}
		}
		else if(!(mtmp->mspec_used)){
			pline("%s sings, and the world ripples and trembles around you.", Monnam(mtmp));
			mtmp->mspec_used = rnd(4);
			switch(rnd(5)){
				case 1:{
				// pline("death\n");
				boolean resisted;
				for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
					if(tmpm != mtmp && !DEADMONSTER(tmpm)){
						if (resists_death(tmpm)) {
							// if (canseemon(tmpm))
								// pline("%s seems no deader than before.", Monnam(tmpm));
						} else if (!(resisted = (resists_magm(tmpm) || resist(tmpm, 0, 0, FALSE))) &&
								   rn2(mtmp->m_lev) > 12) {
								tmpm->mhp = -1;
							monkilled(tmpm, "", AD_SPEL);
						} else {
							if (resisted) shieldeff(tmpm->mx, tmpm->my);
						}
					}
				}
				if (nonliving(youracedata) || is_demon(youracedata)) {
					// You("seem no deader than before.");
				} else if (!Antimagic && (!mtmp || rn2(mtmp->m_lev) > 12) && !(u.sealsActive&SEAL_OSE || resists_death(&youmonst))) {
					if (Hallucination) {
						You("have an out of body experience.");
					} else {
						killer_format = KILLED_BY_AN;
						killer = "song of death";
						done(DIED);
					}
				} else if(!(u.sealsActive&SEAL_OSE)){
					if (Antimagic) shieldeff(u.ux, u.uy);
					Your("%s flutters!", body_part(HEART));
					losehp(rnd(mtmp->m_lev), "song of death", KILLED_BY_AN);
				} else shieldeff(u.ux, u.uy);
				stop_occupation();
				if(uwep && uwep->oartifact == ART_SINGING_SWORD){
					uwep->ovar1_heard |= OHEARD_DEATH;
				}
				}break;
				case 2:{
				// pline("unturn dead\n");
				struct obj *ispe = mksobj(SPE_TURN_UNDEAD, MKOBJ_NOINIT);
				for(ix = 0; ix < COLNO; ix++){
					for(iy = 0; iy < ROWNO; iy++){
						bhitpile(ispe, bhito, ix, iy);
					}
				}
				if(uwep && uwep->oartifact == ART_SINGING_SWORD){
					uwep->ovar1_heard |= OHEARD_LIFE;
				}
				}break;
				case 3:
				// pline("nightmare\n");
				for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
					if(tmpm != mtmp && !DEADMONSTER(tmpm)){
						if(!mindless_mon(tmpm)){
							tmpm->mstun = 1;
							tmpm->mconf = 1;
							tmpm->mberserk = 1;
						}
					}
				}
				ix = mtmp ? rnd((int)mtmp->m_lev) : rnd(10);
				if(Antimagic) ix = (ix + 1) / 2;
				ix = reduce_dmg(&youmonst,ix,FALSE,TRUE);
				make_confused(HConfusion + ix*10, FALSE);
				make_stunned(HStun + ix*5, FALSE);
				make_hallucinated(HHallucination + ix*15, FALSE, 0L);
				stop_occupation();
				if(uwep && uwep->oartifact == ART_SINGING_SWORD){
					uwep->ovar1_heard |= OHEARD_INSANE;
				}
				break;
				case 4:
				// pline("earthquake\n");
				do_earthquake(mtmp->mx, mtmp->my, min(((int)mtmp->m_lev - 1) / 3 + 1,24), min(((int)mtmp->m_lev - 1) / 6 + 1,8), TRUE, mtmp);
				aggravate(); /* wake up without scaring */
				stop_occupation();
				doredraw();
				if(uwep && uwep->oartifact == ART_SINGING_SWORD){
					uwep->ovar1_heard |= OHEARD_QUAKE;
				}
				break;
				case 5:{
				// pline("locking\n");
				struct obj *ispe = mksobj(SPE_WIZARD_LOCK, MKOBJ_NOINIT);
				struct trap *ttmp;
				struct rm *door;
				boolean res = TRUE, vis;
				int loudness = 0;
				const char *msg = (const char *)0;
				const char *dustcloud = "A cloud of dust";
				const char *quickly_dissipates = "quickly dissipates";
				int key;		/* ALI - Artifact doors from slash'em */
				for(ix = 0; ix < COLNO; ix++){
					for(iy = 0; iy < ROWNO; iy++){
						door = &levl[ix][iy];
						ttmp = t_at(ix, iy); /* trap if there is one */
						vis = cansee(ix,iy);
						key = artifact_door(ix, iy);
						if (levl[ix][iy].typ == DRAWBRIDGE_DOWN)
							close_drawbridge(ix,iy);
						if (ttmp && ttmp->ttyp == TRAPDOOR) {
							deltrap(ttmp);
							ttmp = (struct trap *)0;
							newsym(ix, iy);
						}
						bhitpile(ispe, bhito, ix, iy);
						if(!IS_DOOR(door->typ))
					continue;
#ifdef REINCARNATION
						if (Is_rogue_level(&u.uz)) {
							/* Can't have real locking in Rogue, so just hide doorway */
							if (cansee(ix,iy)) pline("%s springs up in the older, more primitive doorway.",
								dustcloud);
							else
								You_hear("a swoosh.");
							if (m_at(ix, iy) || !OBJ_AT(ix,iy)) {
								if (vis) pline_The("cloud %s.",quickly_dissipates);
					continue;
							}
							block_point(ix, iy);
							door->typ = SDOOR;
							if (vis) pline_The("doorway vanishes!");
							newsym(ix,iy);
					continue;
						}
#endif
						// pline("%d %d",ix,iy);
						if (m_at(ix, iy) || OBJ_AT(ix,iy))
					continue;
						/* Don't allow doors to close over traps.  This is for pits */
						/* & trap doors, but is it ever OK for anything else? */
						if (ttmp) {
						/* maketrap() clears doormask, so it should be NODOOR */
					continue;
						}
						switch (((int)door->doormask) & ~D_TRAPPED) {
							case D_CLOSED:
							if (key)
								msg = "The door closes!";
							else{
								msg = "The door locks!";
							}break;
							case D_ISOPEN:
							if (key)
								msg = "The door swings shut!";
							else{
								msg = "The door swings shut, and locks!";
							}break;
							case D_BROKEN:
								msg = "The broken door reassembles and locks!";
							break;
							case D_NODOOR:
							if (key)
								msg = "The broken door reassembles!";
							else{
								msg = "The broken door reassembles and locks!";
							}
							break;
							default:
							res = FALSE;
							break;
						}
						if(res){
							block_point(ix, iy);
							if (key)
								door->doormask = D_CLOSED | (door->doormask & D_TRAPPED);
							else
								door->doormask = D_LOCKED | (door->doormask & D_TRAPPED);
							newsym(ix,iy);
						}
					}
				}
				}break;
			}
		}
	}break;
	case MS_SONG:
	case MS_INTONE:
	case MS_FLOWER:
	{
		struct monst *tmpm;
		struct trap *ttmp;
		int ix, iy, i;
		boolean inrange = FALSE;
		if(noactions(mtmp)) break;
		if((ptr->mtyp == PM_INTONER && !rn2(5)) || ptr->mtyp == PM_BLACK_FLOWER){
			if (!canspotmon(mtmp))
				map_invisible(mtmp->mx, mtmp->my);
			switch(rnd(4)){
				case 1:
					if(ptr->mtyp == PM_INTONER && u.uinsight > Insanity+10) pline("%s screams melodiously.", Monnam(mtmp));
					else pline("%s sings the song of broken eyes.", Monnam(mtmp));
					
					for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
						if(tmpm != mtmp && !DEADMONSTER(tmpm)){
							if(tmpm->mcansee && !tmpm->mblinded){
								tmpm->mcansee = 0;
								tmpm->mblinded = rnd(20);
							}
						}
					}
					
					if(!Blind){
						Your("vision fills with grasping roots!");
						
						make_blinded(Blinded+(long)rnd(20),FALSE);
						if (!Blind) Your1(vision_clears);
					}
				break;
				case 2:
					if(ptr->mtyp == PM_INTONER && u.uinsight > Insanity+10) pline("%s sings a resonant note.", Monnam(mtmp));
					else pline("%s sings a harmless song of ruin.", Monnam(mtmp));
					int trycount;
					for(i = rnd(5); i > 0; i--){
						trycount = 500;
						while(trycount-- > 0){
							ix = rn2(COLNO);
							iy = rn2(ROWNO);
							if(isok(ix,iy) && !(ix == u.ux && iy == u.uy)){
								ttmp = t_at(ix, iy);
								if((levl[ix][iy].typ <= SCORR || levl[ix][iy].typ == CORR || levl[ix][iy].typ == ROOM) && levl[ix][iy].typ != STONE){
									levl[ix][iy].typ = CORR;
									if(!does_block(ix,iy,&levl[ix][iy])) unblock_point(ix,iy);	/* vision:  can see through */
									if(ttmp) {
										if (delfloortrap(ttmp)) ttmp = (struct trap *)0;
									}
									levl[ix][iy].typ = CORR;
									trycount = 0;
								}
							}
						}
					}
					vision_full_recalc = 1;
					doredraw();
				break;
				case 3:{
					struct obj *ispe = mksobj(SPE_TURN_UNDEAD, MKOBJ_NOINIT);
					if(ptr->mtyp == PM_INTONER && u.uinsight > Insanity+10) pline("%s wails deafeningly.", Monnam(mtmp));
					else pline("%s sings the song of the day of repentance.", Monnam(mtmp));
					//Rapture invisible creatures
					for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
						if(tmpm != mtmp && !DEADMONSTER(tmpm) && mtmp->mrevived){
							if(mtmp->minvis && tmpm->perminvis && !get_timer(mtmp->timed, DESUMMON_MON)){
								start_timer(5L, TIMER_MONSTER, DESUMMON_MON, (genericptr_t)tmpm);
							}
						}
					}
					if(vision_full_recalc) doredraw();
					//Make visable creatures invisable
					for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
						if(tmpm != mtmp && !DEADMONSTER(tmpm) && mtmp->mrevived){
							if(!opaque(mtmp->data)){
								mtmp->perminvis = TRUE;
								mtmp->minvis = TRUE;
								newsym(mtmp->mx, mtmp->my);
							} else {
								mtmp->perminvis = TRUE;
								mtmp->minvis = TRUE;
								vision_full_recalc = 1;
							}
						}
					}
					if(vision_full_recalc) doredraw();
					//Resurect creatures:
					for(ix = 0; ix < COLNO; ix++){
						for(iy = 0; iy < ROWNO; iy++){
							bhitpile(ispe, bhito, ix, iy);
						}
					}
				}break;
				case 4:
					if(ptr->mtyp == PM_INTONER && u.uinsight > Insanity+10) pline("%s screams furiously.", Monnam(mtmp));
					else pline("%s sings the song of bloodied prayers.", Monnam(mtmp));
					
					for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
						if(tmpm != mtmp && !DEADMONSTER(tmpm) && mtmp->mpeaceful == tmpm->mpeaceful){
							if(tmpm->mhp < tmpm->mhpmax){
								for(i = (tmpm->mhpmax - tmpm->mhp); i > 0; i--){
									grow_up(tmpm, tmpm);
									//Grow up may have killed mtmp
									if(DEADMONSTER(mtmp))
										break;
								}
							}
						}
					}
				break;
			}
		} else if(!(mtmp->mspec_used) || mtmp->mtyp == PM_INTONER){
			switch(rnd(3)){
				case 1:
					if(mtmp->mtame && distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 5 && !Invulnerable){
						inrange=TRUE;
					} else for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
						if(tmpm != mtmp && !DEADMONSTER(tmpm)){
							if(!mindless_mon(tmpm)){
								if ( mtmp->mpeaceful == tmpm->mpeaceful && distmin(mtmp->mx,mtmp->my,tmpm->mx,tmpm->my) < 5) {
									inrange=TRUE;
								}
							}
						}
					}
					
					if(!inrange) break;
					if (!canspotmon(mtmp) && distmin(u.ux,u.uy,mtmp->mx,mtmp->my) < 5)
						map_invisible(mtmp->mx, mtmp->my);
					if(ptr->mtyp == PM_INTONER && u.uinsight > Insanity+10) pline("%s screeches discordantly.", Monnam(mtmp));
					else pline("%s sings a song of courage.", Monnam(mtmp));
					if(mtmp->mtyp != PM_INTONER) mtmp->mspec_used = rn1(10,10);

					for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
						if(tmpm != mtmp && !DEADMONSTER(tmpm)){
							if(!mindless_mon(tmpm)){
								if ( mtmp->mpeaceful == tmpm->mpeaceful && distmin(mtmp->mx,mtmp->my,tmpm->mx,tmpm->my) < 5) {
									if (tmpm->encouraged < BASE_DOG_ENCOURAGED_MAX)
										tmpm->encouraged = min_ints(BASE_DOG_ENCOURAGED_MAX, tmpm->encouraged + rnd(mtmp->m_lev/3+1));
									if (tmpm->mflee) tmpm->mfleetim = 0;
									if (canseemon(tmpm)) {
										if (Hallucination) {
											if(canspotmon(tmpm)) pline("%s looks %s!", Monnam(tmpm),
												  tmpm->encouraged >= BASE_DOG_ENCOURAGED_MAX ? "way cool" :
												  tmpm->encouraged > (BASE_DOG_ENCOURAGED_MAX/2) ? "cooler" : "cool");
										} else {
											if(canspotmon(tmpm)) pline("%s looks %s!", Monnam(tmpm),
												  tmpm->encouraged >= BASE_DOG_ENCOURAGED_MAX ? "berserk" :
												  tmpm->encouraged > (BASE_DOG_ENCOURAGED_MAX/2) ? "wilder" : "wild");
										}
									}
								}
							}
						}
					}
					if(mtmp->mtame && distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 5 && !Invulnerable){
						if(u.uencouraged < BASE_DOG_ENCOURAGED_MAX) 
							u.uencouraged = min_ints(BASE_DOG_ENCOURAGED_MAX, u.uencouraged + rnd(mtmp->m_lev/3+1));
						You_feel("%s!", u.uencouraged >= BASE_DOG_ENCOURAGED_MAX ? "berserk" : "wild");
					}
					if(distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 5 && uwep && uwep->oartifact == ART_SINGING_SWORD){
						uwep->ovar1_heard |= OHEARD_COURAGE;
					}
				break;
				case 2:
					if(mtmp->mtame && distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 5 && !Invulnerable){
						inrange=TRUE;
					} else for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
						if(tmpm != mtmp && !DEADMONSTER(tmpm)){
							if(!mindless_mon(tmpm)){
								if ( mtmp->mpeaceful == tmpm->mpeaceful && distmin(mtmp->mx,mtmp->my,tmpm->mx,tmpm->my) < 5) {
									if(tmpm->mcan || tmpm->mspec_used || (!tmpm->mnotlaugh && tmpm->mlaughing) || (!tmpm->mcansee && tmpm->mblinded) ||
										tmpm->mberserk || (tmpm->mhp < tmpm->mhpmax) || (!tmpm->mcanmove && tmpm->mfrozen) || tmpm->mstdy > 0 || tmpm->mstun ||
										tmpm->mconf || tmpm->msleeping || tmpm->mflee || tmpm->mfleetim
									) inrange = TRUE;
								}
							}
						}
					}
					
					if(!inrange) break;
					if (!canspotmon(mtmp) && distmin(u.ux,u.uy,mtmp->mx,mtmp->my) < 5)
						map_invisible(mtmp->mx, mtmp->my);
					if(ptr->mtyp == PM_INTONER && u.uinsight > Insanity+10) pline("%s whistles shrilly.", Monnam(mtmp));
					else pline("%s sings a song of good health.", Monnam(mtmp));
					if(mtmp->mtyp != PM_INTONER) mtmp->mspec_used = rn1(10,10);

					for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
						if(tmpm != mtmp && !DEADMONSTER(tmpm)){
							if(!mindless_mon(tmpm)){
								if ( mtmp->mpeaceful == tmpm->mpeaceful && distmin(mtmp->mx,mtmp->my,tmpm->mx,tmpm->my) < 5) {
									set_mcan(tmpm, FALSE);
									tmpm->mspec_used = 0;
									if(!tmpm->mnotlaugh && tmpm->mlaughing){
										tmpm->mnotlaugh = 1;
										tmpm->mlaughing = 0;
									}
									if(!tmpm->mcansee && tmpm->mblinded){
										tmpm->mcansee = 1;
										tmpm->mblinded = 0;
									}
									tmpm->mberserk = 0;
									if(tmpm->mhp < tmpm->mhpmax) tmpm->mhp = min(tmpm->mhp + tmpm->m_lev,tmpm->mhpmax);
									if(!tmpm->mcanmove && tmpm->mfrozen){
										tmpm->mcanmove = 1;
										tmpm->mfrozen = 0;
									}
									if(tmpm->mstdy > 0) tmpm->mstdy = 0;
									tmpm->mstun = 0;
									tmpm->mconf = 0;
									tmpm->msleeping = 0;
									tmpm->mflee = 0;
									tmpm->mfleetim = 0;
								}
							}
						}
					}
					if(mtmp->mtame && distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 5 && !Invulnerable){
						healup(u.ulevel, 0, FALSE, FALSE);
						use_unicorn_horn((struct obj *)0);
					}
					if(distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 5 && uwep && uwep->oartifact == ART_SINGING_SWORD){
						uwep->ovar1_heard |= OHEARD_HEALING;
					}
				break;
				case 3:
					if(mtmp->mtame && distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 5 && u.uhp < u.uhpmax && !Invulnerable){
						inrange=TRUE;
					} else for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
						if(tmpm != mtmp && !DEADMONSTER(tmpm)){
							if(!mindless_mon(tmpm) && tmpm->data->mmove){
								if ( mtmp->mpeaceful == tmpm->mpeaceful && distmin(mtmp->mx,mtmp->my,tmpm->mx,tmpm->my) < 5) {
									inrange = TRUE;
								}
							}
						}
					}
					
					if(!inrange) break;
					if (!canspotmon(mtmp) && distmin(u.ux,u.uy,mtmp->mx,mtmp->my) < 5 && !Invulnerable)
						map_invisible(mtmp->mx, mtmp->my);
					if(ptr->mtyp == PM_INTONER && u.uinsight > Insanity+10) pline("%s laughs frantically.", Monnam(mtmp));
					else pline("%s sings a song of haste.", Monnam(mtmp));
					if(mtmp->mtyp != PM_INTONER) mtmp->mspec_used = rn1(10,10);
					
					for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
						if(tmpm != mtmp && !DEADMONSTER(tmpm)){
							if(!mindless_mon(tmpm) && tmpm->data->mmove){
								if ( mtmp->mpeaceful == tmpm->mpeaceful && distmin(mtmp->mx,mtmp->my,tmpm->mx,tmpm->my) < 5) {
									tmpm->movement += 12;
									tmpm->permspeed = MFAST;
									tmpm->mspeed = MFAST;
									if(canspotmon(tmpm)) pline("%s moves quickly to attack.", Monnam(tmpm));
								}
							}
						}
					}
					if(mtmp->mtame && distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 5 && !Invulnerable){
						pline("That puts a spring in your step.");
						youmonst.movement += 12;
						if(Wounded_legs)
							heal_legs();
						if (!(HFast & INTRINSIC)) {
							if (!Fast) You("speed up.");
							else Your("quickness feels more natural.");
							HFast |= TIMEOUT_INF;
						}
					}
					if(distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 5 && uwep && uwep->oartifact == ART_SINGING_SWORD){
						uwep->ovar1_heard |= OHEARD_HASTE;
					}
				break;
			}
		} else goto humanoid_sound;
	}break;
	case MS_OONA:{
		struct monst *tmpm;
		int dmg;
		boolean inrange = FALSE;
		if(!(mtmp->mspec_used)){
			switch(rnd(3)){
				case 1:
					if(!mtmp->mpeaceful && distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 4 && !Invulnerable){
						inrange=TRUE;
					} else for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
						if(tmpm != mtmp && !DEADMONSTER(tmpm)){
							if ( mtmp->mpeaceful != tmpm->mpeaceful && distmin(mtmp->mx,mtmp->my,tmpm->mx,tmpm->my) < 4) {
								inrange=TRUE;
							}
						}
					}
					if(!mtmp->mpeaceful && distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 4){
						inrange=TRUE;
					}
					
					if(!inrange) break;
					if (!canspotmon(mtmp) && distmin(u.ux,u.uy,mtmp->mx,mtmp->my) < 4)
						map_invisible(mtmp->mx, mtmp->my);
					
					mtmp->mspec_used = rn1(3,3);
					switch(u.oonaenergy){
						case AD_FIRE:
							pline("%s sings the lament of flames.", Monnam(mtmp));
							if(distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 4 && uwep && uwep->oartifact == ART_SINGING_SWORD){
								uwep->ovar1_heard |= OHEARD_FIRE;
							}
						break;
						case AD_COLD:
							pline("%s sings the lament of ice.", Monnam(mtmp));
							if(distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 4 && uwep && uwep->oartifact == ART_SINGING_SWORD){
								uwep->ovar1_heard |= OHEARD_FROST;
							}
						break;
						case AD_ELEC:
							pline("%s sings the lament of storms.", Monnam(mtmp));
							if(distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 4 && uwep && uwep->oartifact == ART_SINGING_SWORD){
								uwep->ovar1_heard |= OHEARD_ELECT;
							}
						break;
					}

					for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
						if(tmpm != mtmp && !DEADMONSTER(tmpm)){
							if ( mtmp->mpeaceful != tmpm->mpeaceful && distmin(mtmp->mx,mtmp->my,tmpm->mx,tmpm->my) < 4 && !resist(tmpm, 0, 0, FALSE)) {
								dmg = 0;
								switch(u.oonaenergy){
									case AD_FIRE:
										if(resists_fire(tmpm)) dmg = d(min(MAX_BONUS_DICE, mtmp->m_lev/3),4);
										else break;
										if(!resists_cold(tmpm)) dmg *= 1.5;
									break;
									case AD_COLD:
										if(resists_cold(tmpm)) dmg = d(min(MAX_BONUS_DICE, mtmp->m_lev/3),4);
										else break;
										if(!resists_fire(tmpm)) dmg *= 1.5;
									break;
									case AD_ELEC:
										if(resists_elec(tmpm)) dmg = d(min(MAX_BONUS_DICE, mtmp->m_lev/3),4);
									break;
								}
								if(dmg) tmpm->mhp = max(tmpm->mhp - dmg,1);
							}
						}
					}
					if(!mtmp->mpeaceful && distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 4 && !Invulnerable){
						dmg = 0;
						switch(u.oonaenergy){
							case AD_FIRE:
								if(Fire_resistance) dmg = d(min(MAX_BONUS_DICE, mtmp->m_lev/3)+10,4);
								else break;
								if(!Cold_resistance) dmg *= 1.5;
							break;
							case AD_COLD:
								if(Cold_resistance) dmg = d(min(MAX_BONUS_DICE, mtmp->m_lev/3)+10,4);
								else break;
								if(!Fire_resistance) dmg *= 1.5;
							break;
							case AD_ELEC:
								if(Shock_resistance) dmg = d(min(MAX_BONUS_DICE, mtmp->m_lev/3)+10,4);
							break;
						}
						dmg = reduce_dmg(&youmonst,dmg,FALSE,TRUE);
						if(dmg) dmg = min(dmg,Upolyd ? (u.mh - 1) : (u.uhp - 1));
						if(dmg) mdamageu(mtmp,dmg);
					}
				break;
				case 2:
					if(!mtmp->mpeaceful && distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 4 && !Invulnerable){
						inrange=TRUE;
					} else for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
						if(tmpm != mtmp && !DEADMONSTER(tmpm)){
							if(!mindless_mon(tmpm)){
								if ( mtmp->mpeaceful != tmpm->mpeaceful && distmin(mtmp->mx,mtmp->my,tmpm->mx,tmpm->my) < 4) {
									inrange = TRUE;
								}
							}
						}
					}
					if(!inrange) break;
					if (!canspotmon(mtmp) && distmin(u.ux,u.uy,mtmp->mx,mtmp->my) < 4)
						map_invisible(mtmp->mx, mtmp->my);
					
					pline("%s sings a dirge.", Monnam(mtmp));
					mtmp->mspec_used = rn1(3,3);
					
					for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
						if(tmpm != mtmp && !DEADMONSTER(tmpm)){
							if(!mindless_mon(tmpm)){
								if ( mtmp->mpeaceful != tmpm->mpeaceful && distmin(mtmp->mx,mtmp->my,tmpm->mx,tmpm->my) < 5) {
									if (tmpm->encouraged > -1*BASE_DOG_ENCOURAGED_MAX)
										tmpm->encouraged = max_ints(-1*BASE_DOG_ENCOURAGED_MAX, tmpm->encouraged - rnd(mtmp->m_lev/3+1));
									if (tmpm->mflee) tmpm->mfleetim = 0;
									if (canseemon(tmpm)) {
										if (Hallucination) {
											if(canspotmon(tmpm)) pline("%s looks %s!", Monnam(tmpm),
												  tmpm->encouraged <= -1*BASE_DOG_ENCOURAGED_MAX ? "peaced out" :
												  tmpm->encouraged < (-1*BASE_DOG_ENCOURAGED_MAX/2) ? "mellower" : "mellow");
										} else {
											if(canspotmon(tmpm)) pline("%s looks %s!", Monnam(tmpm),
												  tmpm->encouraged <= -1*BASE_DOG_ENCOURAGED_MAX ? "inconsolable" :
												  tmpm->encouraged < -1*(BASE_DOG_ENCOURAGED_MAX/2) ? "depressed" : "a bit sad");
										}
									}
								}
							}
						}
					}
					if(!mtmp->mpeaceful && distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 4 && !Invulnerable){
						if(u.uencouraged > -1*BASE_DOG_ENCOURAGED_MAX) 
							u.uencouraged = max_ints(-1*BASE_DOG_ENCOURAGED_MAX, u.uencouraged - rnd(mtmp->m_lev/3+1));
						You_feel("%s!", u.uencouraged <= -1*BASE_DOG_ENCOURAGED_MAX ? "inconsolable" : "depressed");
					}
					if(distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 4 && uwep && uwep->oartifact == ART_SINGING_SWORD){
						uwep->ovar1_heard |= OHEARD_DIRGE;
					}
				break;
				case 3:
					if(!mtmp->mpeaceful && distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 4 && !Invulnerable){
						inrange=TRUE;
					} else for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
						if(tmpm != mtmp && !DEADMONSTER(tmpm)){
							if(!mindless_mon(tmpm) && tmpm->data->mmove){
								if ( mtmp->mpeaceful != tmpm->mpeaceful && distmin(mtmp->mx,mtmp->my,tmpm->mx,tmpm->my) < 4) {
									inrange = TRUE;
								}
							}
						}
					}
					
					if(!inrange) break;
					
					if (!canspotmon(mtmp) && distmin(u.ux,u.uy,mtmp->mx,mtmp->my) < 4)
						map_invisible(mtmp->mx, mtmp->my);
					pline("%s sings a slow march.", Monnam(mtmp));
					mtmp->mspec_used = rn1(3,3);

					for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
						if(tmpm != mtmp && !DEADMONSTER(tmpm)){
							if(!mindless_mon(tmpm) && tmpm->data->mmove){
								if ( mtmp->mpeaceful != tmpm->mpeaceful && distmin(mtmp->mx,mtmp->my,tmpm->mx,tmpm->my) < 4 && !resist(tmpm, 0, 0, FALSE)) {
									tmpm->movement -= 12;
									tmpm->permspeed = MSLOW;
									tmpm->mspeed = MSLOW;
								}
							}
						}
					}
					if(!mtmp->mpeaceful && distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 4 && !Invulnerable){
						pline("Your body feels leaden!");
						youmonst.movement -= 12;
						if ((HFast & TIMEOUT_INF)) {
							HFast &= ~TIMEOUT_INF;
							if (!Fast) You("slow down.");
							else Your("quickness feels less natural.");
						}
					}
					if(distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 4 && uwep && uwep->oartifact == ART_SINGING_SWORD){
						uwep->ovar1_heard |= OHEARD_LETHARGY;
					}
				break;
			}
		} else goto humanoid_sound;
	}break;
	case MS_APOC:
		if(chatting){
			if(!mtmp->mpeaceful) pline_msg = "hisses!";
			else {
				pline_msg = "does not respond.";
				return 0;	/* no sound */
			}
			break;
		}
		else {
			struct monst *tmpm, *nmon;
			int atknum = rnd(4);
			int i, mnum;
			mtmp->mspec_used = 66;
			if(canspotmon(mtmp)){
				pline("%s snake head hisses a prophecy!", s_suffix(Monnam(mtmp)));
			}
			switch(atknum){
				/*Slashing darkness*/
				case 1:
					mnum = PM_INVIDIAK;
					pline("The darkness shifts and forms into blades!");
				break;
				/*Falling stars*/
				case 2:
					mnum = PM_MOTE_OF_LIGHT;
					pline("Stars fall from the sky!");
				break;
				/*Scream*/
				case 3:
					mnum = PM_WALKING_DELIRIUM;
					pline("The world trembles and crawls!");
				break;
				/*Earthquake*/
				case 4:
					mnum = PM_EARTH_ELEMENTAL;
					pline("The entire world is shaking around you!");
				break;
			}
			for(i = Insanity/3; i > 0; i--){
				tmpm = makemon(&mons[mnum], 0, 0, MM_NOCOUNTBIRTH|MM_ESUM);
				if(tmpm){
					mark_mon_as_summoned(tmpm, mtmp, 66, 0);
					if(mnum == PM_MOTE_OF_LIGHT)
						set_template(tmpm, FALLEN_TEMPLATE);
					tmpm->m_lev = 15;
					tmpm->mhpmax = max(4, 8*tmpm->m_lev);
					tmpm->mhp = tmpm->mhpmax;
				}
			}
		}
	break;
	case MS_HOWL:{
		struct monst *tmpm;
	    pline_msg = "howls.";
		(void) makemon((struct permonst *)0, 0, 0, NO_MM_FLAGS);
		for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
			if(tmpm->mtame && rn2(tmpm->mtame + 1)){
				tmpm->mtame--;
				if (!tmpm->mtame)
					untame(tmpm, 1);
			}
		}
	    aggravate();
	}break;
	case MS_SCREAM:{
		struct monst *tmpm;
		if(distmin(u.ux, u.uy, mtmp->mx, mtmp->my) <= BOLT_LIM)
			pline("%s screams in madness and fear!", Monnam(mtmp));
		for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
			if(tmpm != mtmp && !DEADMONSTER(tmpm) && tmpm->mpeaceful != mtmp->mpeaceful && distmin(tmpm->mx, tmpm->my, mtmp->mx, mtmp->my) <= BOLT_LIM){
				if(!resist(tmpm, TOOL_CLASS, 0, FALSE)){
					tmpm->mflee = 1;
					if(canseemon(tmpm))
						pline("%s staggers!", Monnam(tmpm));
					if(tmpm->mhp < mtmp->mhpmax && !resist(tmpm, TOOL_CLASS, 0, FALSE)){
						tmpm->mcrazed = 1;
					}
				}
			}
		}
		if(!mtmp->mpeaceful && distmin(u.ux, u.uy, mtmp->mx, mtmp->my) <= BOLT_LIM){
			change_usanity(u_sanity_loss_minor(mtmp), TRUE);
		}
	    aggravate();
	}break;
	case MS_SHRIEK:
	    pline_msg = "shrieks.";
	    aggravate();
	break;
	case MS_SHOG:
		if(couldsee(mtmp->mx,mtmp->my)){
			verbl_msg = "Tekeli-li! Tekeli-li!";
		} else {
			You_hear("distant piping sounds.");
		}
		aggravate();
	break;
	case MS_SECRETS:
		if(chatting){
			if(mtmp->mtame && mtmp->mtyp == PM_VEIL_RENDER
			 && (
				u.umorgul > 0
				|| Sick
				|| Slimed
				|| u.thoughts
				|| (count_glyphs() < MAX_GLYPHS && !u.render_thought)
			 )
			){
				if(render_services(mtmp))
					break;
			}
			else if(mtmp->mspec_used){
				pline_msg = "whispers.";
				break;
			}
		}
		else {
			if(mtmp->mtame){
				pline("%s whispers dire secrets, filling you with zeal.", Monnam(mtmp));
				u.uencouraged = min_ints(Insanity/5+1, u.uencouraged+rnd(Insanity/5+1));
				exercise(A_INT, TRUE);
				exercise(A_WIS, TRUE);
				exercise(A_CHA, TRUE);
			} else if(!mtmp->mpeaceful){
				aggravate();
			}
			mtmp->mspec_used = 5;
		}
	break;
	case MS_IMITATE:
	    pline_msg = "imitates you.";
	    break;
	case MS_BONES:
	    pline("%s rattles noisily.", Monnam(mtmp));
	    You("freeze for a moment.");
	    nomul(-2, "scared by rattling bones");
	    break;
	case MS_LAUGH:
	    {
		static const char * const laugh_msg[4] = {
		    "giggles.", "chuckles.", "snickers.", "laughs.",
		};
		pline_msg = laugh_msg[rn2(4)];
	    }
	    break;
	case MS_MUMBLE:
	    pline_msg = "mumbles incomprehensibly.";
	    break;
	case MS_DJINNI:
		if(Role_if(PM_EXILE) && In_quest(&u.uz) && ptr->mtyp == PM_PRISONER){
			verbl_msg = woePrisoners[rn2(SIZE(woePrisoners))];
		} else if (ptr->mtyp == PM_EMBRACED_DROWESS) {
			verbl_msg = embracedPrisoners[rn2(SIZE(embracedPrisoners))];
		} else if (ptr->mtyp == PM_PARASITIZED_EMBRACED_ALIDER) {
			verbl_msg = embracedAlider[rn2(SIZE(embracedAlider))];
	    } else if(ptr->mtyp == PM_A_GONE) verbl_msg = agonePrisoner[rn2(SIZE(agonePrisoner))];
	    else if(ptr->mtyp == PM_MINDLESS_THRALL) verbl_msg = thrallPrisoners[rn2(SIZE(thrallPrisoners))];
	    else if(ptr->mtyp == PM_PARASITIZED_ANDROID || ptr->mtyp == PM_PARASITIZED_GYNOID) verbl_msg = parasitizedDroid[rn2(SIZE(parasitizedDroid))];
	    else if (mtmp->mtame) {
			verbl_msg = "Sorry, I'm all out of wishes.";
	    } else if (mtmp->mpeaceful) {
			if (ptr->mtyp == PM_MARID)
				pline_msg = "gurgles.";
			else
				verbl_msg = "I'm free!";
		} else if(ptr->mtyp != PM_PRISONER) verbl_msg = "This will teach you not to disturb me!";
		else verbl_msg = "I'm free!";
	    break;
	case MS_BOAST:	/* giants */
	    if (!mtmp->mpeaceful) {
		switch (rn2(4)) {
		case 0: pline("%s boasts about %s gem collection.",
			      Monnam(mtmp), mhis(mtmp));
			break;
		case 1: pline_msg = "complains about a diet of mutton.";
			break;
	       default: pline_msg = "shouts \"Fee Fie Foe Foo!\" and guffaws.";
			wake_nearto_noisy(mtmp->mx, mtmp->my, 7*7);
			break;
		}
		break;
	    }
	    /* else FALLTHRU */
	case MS_HUMANOID:
humanoid_sound:
		if(Race_if(PM_GNOME) && Role_if(PM_RANGER) && mtmp->mtyp == PM_RUGGO_THE_GNOME_HIGH_KING){
			verbl_msg = "Ah, comrade!  It is good you are here.  I've hidden the angel behind my throne.";
			break;
		}

	    if (!mtmp->mpeaceful) {
			if (In_endgame(&u.uz) && is_mplayer(ptr)) {
				mplayer_talk(mtmp);
				break;
			}
			else return 0;	/* no sound */
	    }
		
	    if (mtmp->mflee)
		pline_msg = "wants nothing to do with you.";
	    else if (mtmp->mhp < mtmp->mhpmax/4)
		pline_msg = "moans.";
	    else if (mtmp->mconf || mtmp->mstun)
		verbl_msg = !rn2(3) ? "Huh?" : rn2(2) ? "What?" : "Eh?";
	    else if (is_blind(mtmp))
		verbl_msg = "I can't see!";
	    else if (mtmp->mtrapped) {
			struct trap *t = t_at(mtmp->mx, mtmp->my);

			if (t) t->tseen = 1;
			verbl_msg = "I'm trapped!";
	    } else if (mtmp->mhp < mtmp->mhpmax/2)
		pline_msg = "asks for a potion of healing.";
	    else if (get_mx(mtmp, MX_EDOG) &&
						moves > EDOG(mtmp)->hungrytime)
		verbl_msg = "I'm hungry.";
	    /* Specific monsters' interests */
	    /* Generic peaceful humanoid behaviour. */
	    else if (mtmp->mpeaceful && uclockwork && !mtmp->mtame && !nohands(ptr) && !is_animal(ptr) && yn("(Ask for help winding your clockwork?)") == 'y'){
			struct obj *key;
			int turns = 0;
			
			Strcpy(class_list, tools);
			key = getobj(class_list, "wind with");
			if (!key){
				pline1(Never_mind);
				break;
			}
			if(!mtmp->mtame) turns = ask_turns(mtmp, u.ulevel*10 + 100, u.ulevel/10+1);
			else turns = ask_turns(mtmp, 0, 0);
			if(!turns){
				pline1(Never_mind);
				break;
			}
			start_clockwinding(key, mtmp, turns);
			break;
		}
		else if(mtmp->mpeaceful && uclockwork && mtmp->mtyp == PM_TINKER_GNOME && yn("(Buy clockwork components?)") == 'y'){
			struct obj *comp;
			int howmany = 0;
			
			if(!mtmp->mtame) howmany = ask_cp(mtmp,100);
			else howmany = ask_cp(mtmp,50);
			if(!howmany){
				pline1(Never_mind);
				break;
			}
			// start_clockwinding(key, mtmp, turns);
			comp = mksobj(CLOCKWORK_COMPONENT, NO_MKOBJ_FLAGS);
			comp->blessed = FALSE;
			comp->cursed = FALSE;
			comp->quan = howmany;
			hold_another_object(comp, "You drop %s!",
				doname(comp), (const char *)0);
			break;
		}
		else if(mtmp->mpeaceful && uclockwork && mtmp->mtyp == PM_HOOLOOVOO && yn("(Buy subethaic components?)") == 'y'){
			struct obj *comp;
			int howmany = 0;
			
			if(!mtmp->mtame) howmany = ask_cp(mtmp,1000);
			else howmany = ask_cp(mtmp,500);
			if(!howmany){
				pline1(Never_mind);
				break;
			}
			// start_clockwinding(key, mtmp, turns);
			comp = mksobj(SUBETHAIC_COMPONENT, NO_MKOBJ_FLAGS);
			comp->blessed = FALSE;
			comp->cursed = FALSE;
			comp->quan = howmany;
			hold_another_object(comp, "You drop %s!",
				doname(comp), (const char *)0);
			break;
		}
	    else {
			const char *talkabt = "talks about %s.";
			const char *discuss = "discusses %s.";
			if((ptr->mtyp == PM_ITINERANT_PRIESTESS || ptr->mtyp == PM_PRIESTESS || ptr->mtyp == PM_DEMINYMPH)
				&& has_template(mtmp, MISTWEAVER)
			){
				if(mtmp->mtame && has_object_type(invent, HOLY_SYMBOL_OF_THE_BLACK_MOTHE)){
					if(!u.shubbie_atten){
						godlist[GOD_THE_BLACK_MOTHER].anger = 0;
						u.shubbie_atten = 1;
					}
					if(godlist[GOD_THE_BLACK_MOTHER].anger == 0){
						pacify_goat_faction();
					}
				}
				switch(rn2(15)){
					case 0:
						verbl_msg = "Ia! Shub-Nugganoth! The Goat with a Thousand Young!";
					break;
					case 1:
						verbl_msg = "Abundance to the Black Goat of the Woods!";
					break;
					case 2:
						verbl_msg = "From the wells of night to the gulfs of space, and from the gulfs of space to the wells of night, ever Their praises!";
					break;
					case 3:
						verbl_msg = "May Her eyes guide you.";
					break;
					case 4:
						verbl_msg = "Gof'nn hupadgh Shub-Nugganoth!";
					break;
					case 5:
					case 6:
						verbl_msg = "Ia!";
					break;
					case 7:
						verbl_msg = "Solve et coagula!";
					break;
					case 8:
						verbl_msg = "We stand on the brink of a strange world.";
					break;
					case 9:
						verbl_msg = "She shall spawn and spawn again!";
					break;
					case 10:
						verbl_msg = "May Her light shine upon you!";
					break;
					case 11:
						verbl_msg = "Neither man nor beast.";
					break;
					case 12:
						verbl_msg = "Neither the living nor the dead.";
					break;
					case 13:
						verbl_msg = "All things are mingled.";
					break;
					case 14:
						verbl_msg = "Silet per diem universus, lucet nocturnis ignibus.";
					break;
				}
			}
			else if(ptr->mtyp == PM_DARK_YOUNG){
				if(mtmp->mtame && has_object_type(invent, HOLY_SYMBOL_OF_THE_BLACK_MOTHE)){
					if(!u.shubbie_atten){
						godlist[GOD_THE_BLACK_MOTHER].anger = 0;
						u.shubbie_atten = 1;
					}
					if(godlist[GOD_THE_BLACK_MOTHER].anger == 0){
						pacify_goat_faction();
					}
				}
				switch(rn2(14)){
					case 0:
						verbl_msg = "Ia! Shub-Nugganoth!";
					break;
					case 1:
						verbl_msg = "Y'ai 'ng'ngah, Yog-Sothoth h'ee - l'geb f'ai throdog uaaah!";
					break;
					case 2:
						verbl_msg = "Ph'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn.";
					break;
					case 3:
						//Extracted from Notebook found in a Deserted House (Robert Bloch)
						verbl_msg = "Ia Shub-Nugganoth! R'lyeh nb'shoggoth!";
					break;
					case 4:
						verbl_msg = "Gof'nn hupadgh Shub-Nugganoth!";
					break;
					case 5:
					case 6:
						verbl_msg = "Ia!";
					break;
					case 7:
						//These three are from the Burrowers Beneath, not by Lovecraft
						verbl_msg = "Ya na kadishtu nilgh'ri stell'bsna Nyogtha.";
					break;
					case 8:
						verbl_msg = "K'yarnak phlegethor l'ebumna syha'h n'ghft.";
					break;
					case 9:
						verbl_msg = "Ya hai kadishtu ep r'luh-eeh Nyogtha eeh.";
					break;
					default:
						pline_msg = "mumbles incoherently.";
					break;
				}
			}
			else switch (monsndx(ptr)) {
				case PM_VALAVI:
					Sprintf(msgbuff, talkabt, rn2(2) ? "herding" : rn2(2) ? "carpentry" : rn2(10) ? "pottery" : "delicious sawdust recipes");
					pline_msg = msgbuff;
				break;
				case PM_DRACAE_ELADRIN:
				case PM_MOTHERING_MASS:
					Sprintf(msgbuff, talkabt, rn2(10) ? "babies" : "stars distant and strange");
					pline_msg = msgbuff;
				break;
				case PM_HOBBIT:
					pline_msg = (mtmp->mhpmax - mtmp->mhp >= 10) ?
						"complains about unpleasant dungeon conditions."
						: "asks you about the One Ring.";
				break;
				case PM_DWARF:
				case PM_DWARF_LORD:
					Sprintf(msgbuff, talkabt, !rn2(4) ? "mining" : !rn2(3) ? "prospecting" : rn2(2) ? "metalwork" : "beer");
					pline_msg = msgbuff;
				break;
				case PM_YURIAN:
					Sprintf(msgbuff, talkabt, "sea gardening");
					pline_msg = msgbuff;
				break;
				case PM_COURE_ELADRIN:
					Sprintf(msgbuff, talkabt, !rn2(4) ? "flowers" : !rn2(3) ? "green grass" : rn2(2) ? "whimsical things" : "the colors of the stars");
					pline_msg = msgbuff;
				break;
				case PM_NOVIERE_ELADRIN:
					Sprintf(msgbuff, talkabt, !rn2(3) ? "sudden storms" : !rn2(2) ? "whirlpools" : rn2(10) ? "starlight on the water" : "the secret and forgotten depths");
					pline_msg = msgbuff;
				break;
				case PM_BRALANI_ELADRIN:
					Sprintf(msgbuff, talkabt, !rn2(4) ? "sudden storms" : !rn2(3) ? "wandering on the gasping dust" : rn2(2) ? "desert flowers" : rn2(10) ? "the stars over the desert sands" : "secret and forgotten ruins");
					pline_msg = msgbuff;
				break;
				case PM_FIRRE_ELADRIN:
					Sprintf(msgbuff, talkabt, !rn2(4) ? "campfire stories" : !rn2(3) ? "fire and light" : !rn2(2) ? "pyromantic augury" : rn2(10) ? "the stars through the flames" : "secret and forgotten stories");
					pline_msg = msgbuff;
				break;
				case PM_GAE_ELADRIN:
					Sprintf(msgbuff, talkabt, !rn2(4) ? "birth and death" : !rn2(3) ? "the changing seasons of life" : rn2(10) ? (!rn2(4) ? "the stars beyond the rains of spring" : !rn2(3) ? "the stars above the green summer canopy" : !rn2(4) ? "stars among the autumn leaves" : "stars seen past the barren branches") : "secret rebirths");
					pline_msg = msgbuff;
				break;
				case PM_FORMIAN_CRUSHER:
				case PM_FORMIAN_TASKMASTER:
					pline_msg = "chitters.";
				break;
				case PM_MARILITH:
					if(rn2(2)){
						Sprintf(msgbuff, talkabt, !rn2(4) ? "swords" : !rn2(3) ? "spears" : rn2(2) ? "bludgeons" : "knives");
						pline_msg = msgbuff;
					}
					else if(!rn2(3))
						pline_msg = "discusses military tactics.";
					else if(rn2(2))
						pline_msg = "curses devils.";
					else
						pline_msg = "curses angels.";
				break;
				case PM_ARCHEOLOGIST:
					pline_msg = "describes a recent article in \"Spelunker Today\" magazine.";
				break;
				case PM_TOURIST:
					verbl_msg = "Aloha.";
				break;
				case PM_LADY_CONSTANCE:
					if(!u.uevent.qcompleted){
						if(!quest_status.fakeleader_greet_1){
							verbl_msg = "You're back! There's a strange woman in the observation ward. She's asking for you....";
							quest_status.fakeleader_greet_1 = TRUE;
						}
						else if(Race_if(PM_ELF) && !quest_status.fakeleader_greet_2){
							verbl_msg = "I was able to find the armor you talked about. Do you... still remember it?";
							quest_status.fakeleader_greet_2 = TRUE;
						}
						else {
							verbl_msg = "Have you talked to that strange woman in the observation ward?";
						}
					}
					else {
						if(!rn2(2)){
							Sprintf(msgbuff, discuss, !rn2(5) ? "Fiore's dagger techniques" : !rn2(4) ? "mentalism" : !rn2(3) ? "theosophy" : rn2(2) ? "the occult" : "your recent dreams");
						}
						else {
							Sprintf(msgbuff, talkabt, !rn2(5) ? "ley lines" : !rn2(4) ? "tectonophysics" : !rn2(3) ? "special relativity" : !rn2(2) ? "archaeology" : rn2(5) ? "the collective unconscious" : rn2(10) ? "her recurring dreams" : "her darkest nightmares");
						}
						pline_msg = msgbuff;
					}
				break;
				case PM_PEN_A_MENDICANT:
				case PM_MENDICANT_SPROW:
				case PM_MENDICANT_DRIDER:
					if(!u.uevent.qcompleted){
						switch(rn2(6)){
							case 0:
								verbl_msg = "The ruling houses are fleeing the city.";
							break;
							case 1:
								verbl_msg = "The surfacers pushed back the demons, but now the kuo toa are attacking!";
							break;
							case 2:
								verbl_msg = "Pen'a whispers that Lolth sacrificed the city to the demon lords.";
							break;
							case 3:
								verbl_msg = "The surfacers have occupied this level, but that doesn't help the commoners below.";
							break;
							case 4:
								verbl_msg = "The dwarf-giant slavers have taken much of the lower levels.";
							break;
							case 5:
								verbl_msg = "The surfacers are right to be afraid. If we fail here, no one will be safe.";
							break;
						}
					}
					else {
						switch(rn2(6)){
							case 0:
								verbl_msg = "We must not let the ruling houses reclaim the city.";
							break;
							case 1:
								verbl_msg = "Pen'a whispers that a demon lord drove the kuo toa against us.";
							break;
							case 2:
								verbl_msg = "Lolth is a blight upon the world. We must be free of her.";
							break;
							case 3:
								verbl_msg = "Holy be the alliance of Pen'a and Ilmater.";
							break;
							case 4:
								verbl_msg = "The dwarf-giants still hold much of the lower levels.";
							break;
							case 5:
								verbl_msg = "Lolth has ceded the city by her actions. For the safety of the surface and the dark she must never regain it.";
							break;
						}
					}
				break;
				case PM_SISTER_T_EIRASTRA:
					if(!quest_status.got_quest){
						verbl_msg = flags.female ? "I'm glad you returned whole, little sister. You must speak to Shuushar."
												 : "I'm glad you returned whole, little brother. You must speak to Shuushar.";
					}
					else if(!u.uevent.qcompleted){
						switch(rn2(6)){
							case 0:
								verbl_msg = flags.female ? "Be safe, little sister."
														 : "Be safe, little brother.";
							break;
							case 1:
								verbl_msg = "Remember your studies, and you will prevail!";
							break;
							case 2:
								verbl_msg = "The perversion of the Ana'auo shames us all.";
							break;
							case 3:
								verbl_msg = "You will face many foes. Consider well the strengths of each!";
							break;
							case 4:
								verbl_msg = "Sight beyond sight.";
							break;
							case 5:
								verbl_msg = flags.female ? "The plague grows worse as we speak.  Hurry, little sister."
														 : "The plague grows worse as we speak.  Hurry, little brother.";
							break;
						}
					}
					else {
						if(!u.uhave.amulet){
							verbl_msg = flags.female ? "Greetings, little sister. How fare you on your quest for the Amulet?"
													 : "Greetings, little brother. How fare you on your quest for the Amulet?";
						}
						else {
							com_pager(226);
						}
					}
				break;
				case PM_SIR_ALJANOR:
					if(!u.uevent.qcompleted){
						switch(rn2(5)){
							case 0:
								verbl_msg = "I've taken charge of this expeditionary force.";
							break;
							case 1:
								verbl_msg = "The demonic incursion has been delt with, but now something drives these kuo-toa against us.";
							break;
							case 2:
								verbl_msg = "I'm sure Lolth regrets that her slavers took me alive.";
							break;
							case 3:
								verbl_msg = "We've occupied as much of the city as possible. Tyr shall send aid!";
							break;
							case 4:
								verbl_msg = "We must drive the dwarf-giant slavers out of the city!";
							break;
						}
					}
					else {
						switch(rn2(5)){
							case 0:
								verbl_msg = "We must work together to save all our peoples!";
							break;
							case 1:
								verbl_msg = "I think a demon lord was behind the attacks.";
							break;
							case 2:
								verbl_msg = "By my honor as a knight of Tyr, God of Justice, Lolth will not have this city!";
							break;
							case 3:
								verbl_msg = "We're holding as much of the city as possible. Tyr shall send aid!";
							break;
							case 4:
								verbl_msg = "We must drive the dwarf-giant slavers out of the city!";
							break;
						}
					}
				break;
				case PM_IKSH_NA_DEVA:
					Sprintf(msgbuff, talkabt, !rn2(4) ? "the virtues of poverty" : !rn2(3) ? "the voices of stones" : rn2(2) ? "lights in the dark" : "stars in the deep");
					pline_msg = msgbuff;
				break;
				case PM_ALIDER:
					if(Race_if(PM_ANDROID))
						verbl_msg = freedAlider[rn2(SIZE(freedAlider))];
				break;
				default:
					if(Role_if(PM_RANGER) && Race_if(PM_GNOME) &&
						mtmp->mtyp == PM_ARCADIAN_AVENGER && 
						mtmp->m_id == quest_status.leader_m_id
					) goto asGuardian; /* Jump up to a different case in this switch statment */
					
					if((Role_if(PM_NOBLEMAN) || Role_if(PM_KNIGHT)) && In_quest(&u.uz)){
						if(Race_if(PM_DWARF)) pline_msg = "talks about fishing.";
						else pline_msg = "talks about farming.";
					}
					else if (is_elf(ptr))
					pline_msg = "curses orcs.";
					else if (is_drow(ptr))
					pline_msg = "curses the pale surface freaks.";
					else if (is_dwarf(ptr))
					pline_msg = "talks about mining.";
					else if (likes_magic(ptr))
					pline_msg = "talks about spellcraft.";
					else if (ptr->mlet == S_CENTAUR)
					pline_msg = "discusses hunting.";
					else {
						pline_msg = "discusses dungeon exploration.";
					}
				break;
			}
	    }
	    break;
	case MS_SEDUCE:
#ifdef SEDUCE
	    if (ptr->mlet != S_NYMPH &&
		could_seduce(mtmp, &youmonst, (struct attack *)0) == 1) {
			(void) doseduce(mtmp);
			break;
	    }
	    if (mtmp->mpeaceful && uclockwork && !mtmp->mtame && !nohands(ptr) && !is_animal(ptr) && yn("(Ask for help winding your clockwork?)") == 'y'){
			struct obj *key;
			int turns = 0;
			
			Strcpy(class_list, tools);
			key = getobj(class_list, "wind with");
			if (!key){
				pline1(Never_mind);
				break;
			}
			if(!mtmp->mtame) turns = ask_turns(mtmp, u.ulevel*11 +111, u.ulevel/10+1);
			else turns = ask_turns(mtmp, 0, 0);
			if(!turns){
				pline1(Never_mind);
				break;
			}
			start_clockwinding(key, mtmp, turns);
			break;
		}
	    switch ((poly_gender() != (int) mtmp->female) ? rn2(3) : 0)
#else
	    switch ((poly_gender() == 0) ? rn2(3) : 0)
#endif
	    {
		case 2:
			verbl_msg = "Hello, sailor.";
			break;
		case 1:
			pline_msg = "comes on to you.";
			break;
		default:
			pline_msg = "cajoles you.";
	    }
	    break;
	case MS_ARREST:
	    if (mtmp->mpeaceful)
		verbalize("Just the facts, %s.",
		      flags.female ? "Ma'am" : "Sir");
	    else {
		static const char * const arrest_msg[3] = {
		    "Anything you say can be used against you.",
		    "You're under arrest!",
		    "Stop in the name of the Law!",
		};
		verbl_msg = arrest_msg[rn2(3)];
	    }
	    break;
	case MS_BRIBE:
#ifdef CONVICT        
        if (monsndx(ptr) == PM_PRISON_GUARD) {
            long gdemand = 500 * u.ulevel;
            long goffer = 0;

    	    if (!mtmp->mpeaceful && !mtmp->mtame) {
                pline("%s demands %ld %s to avoid re-arrest.",
                 Amonnam(mtmp), gdemand, currency(gdemand));
                if ((goffer = bribe(mtmp)) >= gdemand) {
                    verbl_msg = "Good.  Now beat it, scum!";
            	    mtmp->mpeaceful = 1;
            	    set_malign(mtmp);
                    break;
                } else {
                    pline("I said %ld!", gdemand);
                    mtmp->mspec_used = 1000;
                    break;
                }
            } else {
                verbl_msg = "Out of my way, scum!"; /* still a jerk */
            }
        } else
#endif /* CONVICT */
	    if (mtmp->mpeaceful) {
			if(!mtmp->mtame) (void) demon_talk(mtmp);
			break;
	    }
	    /* fall through */
	case MS_CUSS:
	    if (!mtmp->mpeaceful)
		cuss(mtmp);
	    break;
	case MS_SPELL:
	    /* deliberately vague, since it's not actually casting any spell */
	    pline_msg = "seems to mutter a cantrip.";
	    break;
	case MS_STATS:
	    if (mtmp->mpeaceful && uclockwork && !mtmp->mtame && !nohands(ptr) && !is_animal(ptr) && yn("(Ask for help winding your clockwork?)") == 'y'){
			struct obj *key;
			int turns = 0;
			
			Strcpy(class_list, tools);
			key = getobj(class_list, "wind with");
			if (!key){
				pline1(Never_mind);
				break;
			}
			if(!mtmp->mtame) turns = ask_turns(mtmp, 0, u.ulevel/15+1);
			else turns = ask_turns(mtmp, 0, 0);
			if(!turns){
				pline1(Never_mind);
				break;
			}
			start_clockwinding(key, mtmp, turns);
			break;
		}
		if(mtmp->mpeaceful){
			if(buy_dolls(mtmp)){
				return TRUE; //mtmp may now be dead
			}
		}
		if (chatting) pline_msg = "does not respond.";
	break;
	case MS_NURSE:
	    if (mtmp->mpeaceful && uclockwork && !mtmp->mtame && !nohands(ptr) && !is_animal(ptr) && yn("(Ask for help winding your clockwork?)") == 'y'){
			struct obj *key;
			int turns = 0;
			
			Strcpy(class_list, tools);
			key = getobj(class_list, "wind with");
			if (!key){
				pline1(Never_mind);
				break;
			}
			if(!mtmp->mtame) turns = ask_turns(mtmp, 0, u.ulevel/15+1);
			else turns = ask_turns(mtmp, 0, 0);
			if(!turns){
				pline1(Never_mind);
				break;
			}
			start_clockwinding(key, mtmp, turns);
			break;
		}
	    if (uwep && (uwep->oclass == WEAPON_CLASS || is_weptool(uwep)))
			verbl_msg = "Put that weapon away before you hurt someone!";
	    else if (uarmc || uarm || uarmh || uarms || uarmg || uarmf)
			verbl_msg = Role_if(PM_HEALER) ?
			  "Doc, I can't help you unless you cooperate." :
			  "Please undress so I can examine you.";
	    else if (uarmu)
			verbl_msg = "Take off your shirt, please.";
		else if(nonliving(youracedata)){
			if(uandroid)
				verbl_msg = "Wow, you're so lifelike! I think you want a mechanic, though.";
			else if(uclockwork)
				verbl_msg = "Oh, not my area of expertise! Try a mechanic!";
			else
				verbl_msg = "Oops, not my department!";
		}
		else if(mtmp->mpeaceful){
			if(nurse_services(mtmp))
				break;
		}
	    else verbl_msg = "Relax, this won't hurt a bit.";
	    break;
	case MS_GUARD:
#ifndef GOLDOBJ
	    if (u.ugold)
#else
	    if (money_cnt(invent))
#endif
		verbl_msg = "Please drop that gold and follow me.";
	    else
		verbl_msg = "Please follow me.";
	    break;
	case MS_SOLDIER:
	    {
		static const char * const soldier_foe_msg[3] = {
		    "Resistance is useless!",
		    "You're dog meat!",
		    "Surrender!",
		},		  * const soldier_pax_msg[3] = {
		    "What lousy pay we're getting here!",
		    "The food's not fit for Orcs!",
		    "My feet hurt, I've been on them all day!",
		};
	    if (mtmp->mpeaceful && uclockwork && !mtmp->mtame && !nohands(ptr) && !is_animal(ptr) && yn("(Ask for help winding your clockwork?)") == 'y'){
			struct obj *key;
			int turns = 0;
			
			Strcpy(class_list, tools);
			key = getobj(class_list, "wind with");
			if (!key){
				pline1(Never_mind);
				break;
			}
			if(!mtmp->mtame) turns = ask_turns(mtmp, u.ulevel*20+200, 0);
			else turns = ask_turns(mtmp, 0, 0);
			if(!turns){
				pline1(Never_mind);
				break;
			}
			start_clockwinding(key, mtmp, turns);
			break;
		}
		verbl_msg = mtmp->mpeaceful ? soldier_pax_msg[rn2(3)]
					    : soldier_foe_msg[rn2(3)];
	    }
	    break;
	case MS_RIDER:
	    if (ptr->mtyp == PM_DEATH && !rn2(10))
		pline_msg = "is busy reading a copy of Sandman #8.";
	    else verbl_msg = "Who do you think you are, War?";
    break;
	default:
		if (chatting) pline_msg = "does not respond.";
	break;
    }

    if (pline_msg) pline("%s %s", Monnam(mtmp), pline_msg);
    else if (verbl_msg) verbalize1(verbl_msg);

	if(chatting && HAS_ESMT(mtmp) && mtmp->mpeaceful){
		char query[BUFSZ] = "";
		if(mtmp->mtyp == PM_DRACAE_ELADRIN)
			Sprintf(query, "Ask %s about incubation services?", mhim(mtmp));
		else
			Sprintf(query, "Ask %s about smithing services?", mhim(mtmp));
		if(yn(query) == 'y'){
			smithy_services(mtmp);
		}
	}

    return(1);
}

static const short command_chain[][2] = {
	{ PM_ORC, PM_ORC_CAPTAIN }, { PM_HILL_ORC, PM_ORC_CAPTAIN }, { PM_MORDOR_ORC, PM_ORC_CAPTAIN },
	{ PM_ORC_CAPTAIN, PM_BOLG },
	{ PM_URUK_HAI, PM_URUK_CAPTAIN },
	{ PM_ORC_CAPTAIN, PM_MORDOR_MARSHAL },
	{ PM_MORDOR_MARSHAL, PM_NAZGUL }, { PM_OLOG_HAI, PM_NAZGUL }, { PM_URUK_CAPTAIN, PM_NAZGUL },
	{ PM_NAZGUL, PM_NECROMANCER },

	{ PM_ANGBAND_ORC, PM_ORC_OF_THE_AGES_OF_STARS},

	{ PM_JUSTICE_ARCHON, PM_RAZIEL }, { PM_SWORD_ARCHON, PM_RAZIEL }, { PM_SHIELD_ARCHON, PM_RAZIEL },

	{ PM_PLAINS_CENTAUR, PM_CENTAUR_CHIEFTAIN }, { PM_FOREST_CENTAUR, PM_CENTAUR_CHIEFTAIN }, { PM_MOUNTAIN_CENTAUR, PM_CENTAUR_CHIEFTAIN },

	{ PM_MIGO_WORKER, PM_MIGO_SOLDIER }, { PM_MIGO_SOLDIER, PM_MIGO_PHILOSOPHER }, { PM_MIGO_PHILOSOPHER, PM_MIGO_QUEEN },

	{ PM_SOLDIER, PM_SERGEANT }, { PM_SERGEANT, PM_LIEUTENANT }, { PM_LIEUTENANT, PM_CAPTAIN },
	{ PM_CAPTAIN, PM_CROESUS },

	{ PM_LEGION_DEVIL_GRUNT, PM_LEGION_DEVIL_SOLDIER }, { PM_LEGION_DEVIL_SOLDIER, PM_LEGION_DEVIL_SERGEANT }, { PM_LEGION_DEVIL_SERGEANT, PM_LEGION_DEVIL_CAPTAIN },
	{ PM_LEGION_DEVIL_CAPTAIN, PM_PIT_FIEND }, { PM_LEGION_DEVIL_CAPTAIN, PM_NESSIAN_PIT_FIEND },
	{ PM_LEMURE, PM_PIT_FIEND }, { PM_IMP, PM_PIT_FIEND }, 
	{ PM_HORNED_DEVIL, PM_PIT_FIEND }, { PM_ERINYS, PM_PIT_FIEND }, { PM_BARBED_DEVIL, PM_PIT_FIEND }, { PM_BONE_DEVIL, PM_PIT_FIEND }, { PM_ICE_DEVIL, PM_PIT_FIEND }, 
	{ PM_LEMURE, PM_NESSIAN_PIT_FIEND }, { PM_IMP, PM_NESSIAN_PIT_FIEND },
	{ PM_HORNED_DEVIL, PM_NESSIAN_PIT_FIEND }, { PM_ERINYS, PM_NESSIAN_PIT_FIEND }, { PM_BARBED_DEVIL, PM_NESSIAN_PIT_FIEND }, { PM_BONE_DEVIL, PM_NESSIAN_PIT_FIEND }, { PM_ICE_DEVIL, PM_NESSIAN_PIT_FIEND }, 
	{ PM_PIT_FIEND, PM_BAEL },
	{ PM_PIT_FIEND, PM_GREEN_PIT_FIEND },
	{ PM_PIT_FIEND, PM_ASMODEUS }, { PM_NESSIAN_PIT_FIEND, PM_ASMODEUS },
	
	{ PM_MANES, PM_MARILITH }, { PM_QUASIT, PM_MARILITH }, { PM_VROCK, PM_MARILITH }, { PM_HEZROU, PM_MARILITH }, { PM_MARILITH, PM_SHAKTARI }, 

	{ PM_MYRMIDON_HOPLITE, PM_MYRMIDON_LOCHIAS }, { PM_MYRMIDON_LOCHIAS, PM_MYRMIDON_YPOLOCHAGOS }, { PM_MYRMIDON_YPOLOCHAGOS, PM_MYRMIDON_LOCHAGOS },
	{ PM_GIANT_ANT, PM_FORMIAN_TASKMASTER }, { PM_FIRE_ANT, PM_FORMIAN_TASKMASTER }, { PM_SOLDIER_ANT, PM_FORMIAN_TASKMASTER },
	{ PM_FORMIAN_CRUSHER, PM_FORMIAN_TASKMASTER }, { PM_MYRMIDON_LOCHIAS, PM_FORMIAN_TASKMASTER },

	{ PM_FERRUMACH_RILMANI, PM_STANNUMACH_RILMANI },

	{ PM_WATCHMAN, PM_WATCH_CAPTAIN },

	{ PM_ANDROID, PM_GYNOID }, { PM_GYNOID, PM_OPERATOR }, { PM_OPERATOR, PM_COMMANDER }, 

	{ NON_PM, NON_PM }

};

boolean
permon_in_command_chain(follower, commander)
int follower;
int commander;
{
	int i;

	switch (commander)	// for special cases
	{
	case PM_LEGION:
	case PM_LEGIONNAIRE:
		impossible("permon_in_command_chain failed for legion(naire)");
		return FALSE;

	default:
		for (i = 0; command_chain[i][0] >= LOW_PM; i++)
		if (follower == command_chain[i][0]) {
			if (commander == command_chain[i][1])
				return TRUE;
			else
				return permon_in_command_chain(command_chain[i][1], commander);
		}
		break;
	}
	return FALSE;
}

boolean
mon_in_command_chain(follower, commander)
struct monst * follower;
struct monst * commander;
{
	if(has_template(commander, MOLY_TEMPLATE)){
		if(is_cha_demon(follower->data))
			return TRUE;
		else return FALSE;
	}
	// else 
	switch (monsndx(commander->data))	// for special cases
	{
	case PM_LEGION:
	case PM_LEGIONNAIRE:
		return (has_template(follower, ZOMBIFIED));

	default:
		return permon_in_command_chain(monsndx(follower->data), monsndx(commander->data));
	}
	return FALSE;
}

// monster commands its followers to fight stronger
void
m_command(commander)
struct monst * commander;
{
	struct monst * mtmp;
	struct monst * nxtmon;
	int tmp = 0;
	int utmp = 0;
	int affected = 0;
	int inrange = 0;
	int nd=1, sd=1;

	switch (monsndx(commander->data))
	{
	case PM_RAZIEL:
		nd=3;
		sd=7;
		break;
	case PM_BAEL:
		nd=2;
		sd=9;
		break;
	case PM_ASMODEUS:
		nd=9;
		sd=1;
		break;
	case PM_NECROMANCER:
		nd=2;
		sd=6;
		break;
	case PM_SERGEANT:
	case PM_MYRMIDON_LOCHIAS:
		nd=1;
		sd=3;
		break;
	case PM_PIT_FIEND:
	case PM_NESSIAN_PIT_FIEND:
	case PM_GREEN_PIT_FIEND:
		nd=1;
		sd=9;
		break;
	case PM_MARILITH:
		nd=1;
		sd=6;
		break;
	case PM_SHAKTARI:
		nd=6;
		sd=1;
		break;
	default:
		nd=1;
		sd=5 + min(30, commander->m_lev)/6;
		break;
	}
	
	for (mtmp = fmon; mtmp; mtmp = nxtmon){
		nxtmon = mtmp->nmon;
		if (!clear_path(mtmp->mx, mtmp->my, commander->mx, commander->my)
			|| (mtmp == commander)
			|| !mon_in_command_chain(mtmp, commander)
			|| !(mtmp->mpeaceful == commander->mpeaceful && !mtmp->mtame == !commander->mtame))
			continue;

		tmp = d(nd, sd);

		inrange += 1;
		if (tmp > mtmp->encouraged || mtmp->mflee){
			mtmp->encouraged = max(tmp, mtmp->encouraged);
			mtmp->mflee = 0;
			mtmp->mfleetim = 0;
			affected += 1;
		}
	}
	if(commander->mtame && clear_path(u.ux, u.uy, commander->mx, commander->my) && permon_in_command_chain(monsndx(youracedata), monsndx(commander->data))){
		inrange += 1;
		utmp = d(nd, sd);
		if(utmp > u.uencouraged)
			affected += 1;
		else utmp = 0;
	}
	if (affected && !(is_silent_mon(commander))) {
		if (canseemon(commander)) {
			switch (monsndx(commander->data))
			{
			case PM_RAZIEL:
				// only messages for large groups
				if (inrange > 4 && (affected > 4 || !rn2(5 - affected))){
					if (affected == inrange)
						pline("%s calls his %s to battle!", Monnam(commander), (inrange<10) ? "host" : "hosts");
					else
						pline("%s rallies his %s!", Monnam(commander), (inrange<10) ? "host" : "hosts");
				}
				break;
			case PM_BAEL:
				// only messages for large groups
				if (inrange > 4 && (affected > 4 || !rn2(5 - affected))){
					if (affected == inrange)
						pline("%s calls his %s to battle!", Monnam(commander), (inrange < 10) ? "legion" : "legions");
					else
						pline("%s rallies his %s!", Monnam(commander), (inrange<10) ? "legion" : "legions");
				}
				break;
			case PM_ASMODEUS:
				// only messages for large groups
				if (inrange > 4 && (affected > 4 || !rn2(5 - affected))){
					if (affected == inrange)
						pline("%s calls his %s to battle!", Monnam(commander), (inrange < 10) ? "subjects" : "infernal hosts");
					else
						pline("%s rallies his %s!", Monnam(commander), (inrange<10) ? "subjects" : "infernal hosts");
				}
				break;
			case PM_LEGION:
			case PM_LEGIONNAIRE:
				//silent
				break;
			default:
				// hide message when few monsters are affected
				if (affected > 4 || !rn2(5 - affected) || affected == inrange){
					if (is_orc(commander->data) || is_demon(commander->data) || is_drow(commander->data))
						pline("%s curses and urges %s follower%s on.", Monnam(commander), mhis(commander), (inrange > 1) ? "s" : "");
					else if (is_mercenary(commander->data))
						pline("%s orders %s %s forwards.", Monnam(commander), mhis(commander), (inrange < 20) ? (inrange < 4) ? "unit" : "forces" : "army");
					else if (!(is_silent_mon(commander)))
						pline("%s gives an order to attack.", Monnam(commander));
				}
				break;
			}
		} else {
			switch (monsndx(commander->data))
			{
			case PM_RAZIEL:
			case PM_BAEL:
				// only messages for large groups
				if (inrange > 4 && (affected > 4 || !rn2(5 - affected)) && distmin(commander->mx, commander->my, u.ux, u.uy) < BOLT_LIM){
					if (affected == inrange)
						You_hear((!Hallucination) ? "a call to battle!" : "a call to the table!");
					else
						You_hear((!Hallucination) ? "a rally cry!" : "a rally car!");
				}
				break;
			case PM_LEGION:
			case PM_LEGIONNAIRE:
				//silent
				break;
			default:
				// hide message when few monsters are affected
				if ((affected > 4 || !rn2(5 - affected) || affected == inrange) && distmin(commander->mx, commander->my, u.ux, u.uy) < BOLT_LIM){
					if (is_orc(commander->data) || is_demon(commander->data) || is_drow(commander->data))
						You_hear((!Hallucination) ? "something cursing." : "mean words.");
					else if (is_mercenary(commander->data))
						You_hear((!Hallucination) ? "soldiers being ordered forwards." : "the Colonel shouting!");
					else
						You_hear((!Hallucination) ? "something give an order to attack." : "something gesture loudly!");
				}
				break;
			}
		}
	}
	if(utmp){
		You("feel inspired!");
		u.uencouraged = utmp;
	}
	return;
}

int
dotalk()
{
    int result;
    boolean save_soundok = flags.soundok;
    flags.soundok = 1;	/* always allow sounds while chatting */
    result = dochat(TRUE, 0, 0, 0);
    flags.soundok = save_soundok;
    return result;
}

int
dochat(ask_for_dir, dx, dy, dz)
boolean ask_for_dir;
int dx;
int dy;
int dz;
{
    register struct monst *mtmp;
    register int tx,ty,bindresult;
    struct obj *otmp;
	
    if (is_silent(youracedata)) {
		pline("As %s, you cannot speak.", an(youracedata->mname));
		return MOVE_CANCELLED;
    }
    if (Strangled) {
		You_cant("speak.  You're choking!");
		return MOVE_CANCELLED;
    }
    else if (Babble) {
		You_cant("communicate.  You're babbling unintelligibly!");
		return MOVE_CANCELLED;
    }
    else if (Screaming) {
		You_cant("communicate.  You're too busy screaming!");
		return MOVE_CANCELLED;
    }
	
	if(mad_turn(MAD_TOO_BIG)){
		pline("It's too big!");
		return MOVE_CANCELLED;
	}
	
    if (u.uswallow) {
		pline("They won't hear you out there.");
		return MOVE_CANCELLED;
    }

    if (!Blind && (otmp = shop_object(u.ux, u.uy)) != (struct obj *)0) {
		/* standing on something in a shop and chatting causes the shopkeeper
		   to describe the price(s).  This can inhibit other chatting inside
		   a shop, but that shouldn't matter much.  shop_object() returns an
		   object iff inside a shop and the shopkeeper is present and willing
		   (not angry) and able (not asleep) to speak and the position contains
		   any objects other than just gold.
		*/
		price_quote(otmp);
//		return(1); //proceed with chat code (maybe you want to speak to the shopkeep about something else, maybe not. shouldn't block either way)
	}

	if (!ask_for_dir) {
		u.dx = dx;
		u.dy = dy;
		u.dz = dz;
	} else if (!getdir("Talk to whom? (in what direction)")) {
		/* decided not to chat */
		return MOVE_CANCELLED;
	}

#ifdef STEED
    if (u.usteed && u.dz > 0)
	return (domonnoise(u.usteed, TRUE)) ? MOVE_STANDARD : MOVE_INSTANT;
#endif
	if (u.dz) {
		struct engr *ep = get_head_engr();
		for(;ep;ep=ep->nxt_engr)
			if(ep->engr_x==u.ux && ep->engr_y==u.uy)
				break;//else continue
		if(!ep || ep->halu_ward || ep->ward_id < FIRST_SEAL) pline("They won't hear you %s there.", u.dz < 0 ? "up" : "down");
		else pline("The gate won't open with you standing on the seal!");
		return MOVE_CANCELLED;
	}

	if (u.dx == 0 && u.dy == 0) {
/*
 * Let's not include this.  It raises all sorts of questions: can you wear
 * 2 helmets, 2 amulets, 3 pairs of gloves or 6 rings as a marilith,
 * etc...  --KAA
	if (u.umonnum == PM_ETTIN) {
	    You("discover that your other head makes boring conversation.");
	    return MOVE_STANDARD;
	}
*/
		pline("Talking to yourself is a bad habit for a dungeoneer.");
		return MOVE_CANCELLED;
    }

    tx = u.ux+u.dx; ty = u.uy+u.dy;
	if (!isok(tx, ty)) return MOVE_CANCELLED;
    mtmp = m_at(tx, ty);
	
	if(In_quest(&u.uz) && urole.neminum == PM_DURIN_S_BANE && artifact_door(tx, ty)){
		char buf[BUFSZ];
		getlin("speak 'Friend' and enter:", buf);
		if(strcmp(buf, "Mellon") == 0){
			register struct rm *here;
			here = &levl[tx][ty];
			here->doormask = D_ISOPEN;
			unblock_point(tx,ty);
			newsym(tx,ty);
		}
		return MOVE_STANDARD;
	}

	bindresult = dobinding(tx,ty);
	if(bindresult != MOVE_CANCELLED) return bindresult;
	
	if(!mtmp && (u.specialSealsActive&SEAL_ACERERAK) &&
		(otmp = level.objects[tx][ty]) && 
		 otmp->otyp == CORPSE && !mindless(&mons[otmp->corpsenm]) && 
		 !is_animal(&mons[otmp->corpsenm])
	){
		You("speak to the shadow that dwells within this corpse.");
		if(otmp->ovar1_corpseRumorCooldown < moves){
			outrumor(rn2(2), BY_OTHER);
			otmp->ovar1_corpseRumorCooldown = moves + rnz(100);
		}
		else pline("....");
	}
	
	if(mtmp && mtmp->mtyp == PM_PRIEST_OF_AN_UNKNOWN_GOD){
	  if(uwep && offerable_artifact(uwep)) {
			struct obj *optr;
			You_feel("%s tug gently on your %s.",mon_nam(mtmp), xname(uwep));
			if(yn("Release it?")=='n'){
				You("hold on tight.");
			}
			else{
				You("let %s take your %s.",mon_nam(mtmp), xname(uwep));
				pline_The(Hallucination ? "world pats you on the head." : "world quakes around you.  Perhaps it is the voice of a god?");
				do_earthquake(u.ux, u.uy, 10, 2, FALSE, (struct monst *)0);
				optr = uwep;
				uwepgone();
				if(optr->gifted != GOD_NONE && optr->gifted != GOD_THE_VOID){
					gods_angry(optr->gifted);
					gods_upset(optr->gifted);
				}
				useup(optr);
				u.regifted++;
				mongone(mtmp);
				if(u.regifted == 5){
					u.uevent.uunknowngod = 1;
					give_ugwish_trophy();
					You_feel("worthy.");
					if (Role_if(PM_EXILE))
					{
						pline("The image of an unknown and strange seal fills your mind!");
						u.specialSealsKnown |= SEAL_UNKNOWN_GOD;
					}
				}
				return MOVE_STANDARD;
			}
	  }
	}
	
	if(mtmp && mtmp->data->msound == MS_GLYPHS){
		if(uwep && offerable_artifact(uwep)
			&& count_glyphs() < MAX_GLYPHS && !(u.thoughts & mtyp_to_thought(mtmp->mtyp))
		){
			struct obj *optr;
			if(canspotmon(mtmp)){
				You_feel("that %s desires your %s.",mon_nam(mtmp), xname(uwep));
			} else {
				You_feel("something desires your %s.",xname(uwep));
			}
			if(yn("Offer it?")=='n'){
				You("refuse.");
				return MOVE_STANDARD;
			}
			else{
				You("let %s take your %s.",mon_nam(mtmp), xname(uwep));
				if (!mtyp_to_thought(mtmp->mtyp))
					return MOVE_CANCELLED;	/* error */
				else
					give_thought(mtyp_to_thought(mtmp->mtyp));

				optr = uwep;
				uwepgone();
				if(optr->gifted != GOD_NONE && optr->gifted != GOD_THE_VOID){
					gods_angry(optr->gifted);
					gods_upset(optr->gifted);
				}
				useup(optr);
				mongone(mtmp);
				return MOVE_STANDARD;
			}
		}
	}
	
	if(mtmp && mtmp->data->msound == MS_UNCURSE){
		int gold, blessing;
#ifndef GOLDOBJ
		gold = u.ugold;
#else
		gold = money_cnt(invent);
#endif
		blessing = doblessmenu();
		if(blessing){
			struct obj *optr;
			int cost;
			switch(blessing){
				case BLESS_CURSES:
					cost = 7;
					if(gold < cost){
						pline("Not enough gold!");
						return MOVE_STANDARD;
					}
					if(yn("That costs 7 gold.  Pay?") != 'y'){
						return MOVE_STANDARD;
					}
#ifndef GOLDOBJ
					u.ugold -= cost;
#else
					money2none(cost);
#endif
					if (Hallucination)
						You_feel("in touch with the Universal Oneness.");
					else
						You_feel("like someone is helping you.");
					for (optr = invent; optr; optr = optr->nobj) {
						uncurse(optr);
					}
					if(Punished) unpunish();
				break;
				case BLESS_LUCK:
					cost = 70;
					if(gold < cost){
						pline("Not enough gold!");
						return MOVE_STANDARD;
					}
					if(yn("That costs 70 gold.  Pay?") != 'y'){
						return MOVE_STANDARD;
					}
#ifndef GOLDOBJ
					u.ugold -= cost;
#else
					money2none(cost);
#endif
					change_luck(2*LUCKMAX);
				break;
				case BLESS_WEP:
					cost = 700;
					if(gold < cost){
						pline("Not enough gold!");
						return MOVE_STANDARD;
					}
					if(yn("That costs 700 gold.  Pay?") != 'y'){
						return MOVE_STANDARD;
					}
					if(!uwep){
						impossible("Your weapon vanished between the menu and the blessing?");
					}
#ifndef GOLDOBJ
					u.ugold -= cost;
#else
					money2none(cost);
#endif
					bless(uwep);
					if((uwep->oclass == WEAPON_CLASS || is_weptool(uwep)) && uwep->spe < 3)
						uwep->spe++;
				break;
				case UNSTERILIZE:
					cost = 7000;
					if(gold < cost){
						pline("Not enough gold!");
						return MOVE_STANDARD;
					}
					if(yn("That costs 7,000 gold.  Pay?") != 'y'){
						return MOVE_STANDARD;
					}
#ifndef GOLDOBJ
					u.ugold -= cost;
#else
					money2none(cost);
#endif
					HSterile = 0L;
				break;
				case SANCTIFY_WEP:
					cost = 70000;
					if(gold < cost){
						pline("Not enough gold!");
						return MOVE_STANDARD;
					}
					if(yn("That costs 70,000 gold.  Pay?") != 'y'){
						return MOVE_STANDARD;
					}
					if(!uwep){
						impossible("Your weapon vanished between the menu and the blessing?");
						break;
					}
#ifndef GOLDOBJ
					u.ugold -= cost;
#else
					money2none(cost);
#endif
					bless(uwep);
					remove_oprop(uwep, OPROP_LESSER_HOLYW);
					if(accepts_weapon_oprops(uwep))
						add_oprop(uwep, OPROP_HOLYW);
					if(uwep->oclass == ARMOR_CLASS)
						add_oprop(uwep, OPROP_HOLY);
					if(uwep->spe < 3)
						uwep->spe = 3;
					mongone(mtmp);
				break;
			}
			update_inventory();
			return MOVE_STANDARD;
		}
		return MOVE_STANDARD;
	}
	
    if ( (!mtmp || mtmp->mundetected ||
		mtmp->m_ap_type == M_AP_FURNITURE ||
		mtmp->m_ap_type == M_AP_OBJECT) && levl[tx][ty].typ == IRONBARS
	){
		tx = tx+u.dx; ty = ty+u.dy;
		mtmp = m_at(tx, ty);
	}
	
	if(mtmp && noactions(mtmp)){
		if(mtmp->mtrapped && t_at(mtmp->mx, mtmp->my) && t_at(mtmp->mx, mtmp->my)->ttyp == VIVI_TRAP){
			if(canspotmon(mtmp))
				pline("%s is sleeping peacefully; presumably the doing of the delicate equipment that displays %s vivisected form.", 
					Monnam(mtmp), (is_animal(mtmp->data) || mindless_mon(mtmp) ? "its" : hisherits(mtmp))
				);
		}
		else if(mtmp->entangled_otyp == SHACKLES){
			if(canspotmon(mtmp))
				pline("%s is unconscious.",  Monnam(mtmp));
		}
		else {
			if(canspotmon(mtmp))
				pline("%s struggles against %s bindings.", 
					Monnam(mtmp), (is_animal(mtmp->data) || mindless_mon(mtmp) ? "its" : hisherits(mtmp))
				);
		}
		return MOVE_INSTANT;
	}
	
    if (!mtmp || mtmp->mundetected ||
		mtmp->m_ap_type == M_AP_FURNITURE ||
		mtmp->m_ap_type == M_AP_OBJECT
	){
		You("don't see anyone to talk to there.");
		return MOVE_CANCELLED;
	}
	
    if (Underwater) {
	Your("speech is unintelligible underwater.");
	return MOVE_INSTANT;
    }

    /* paralized monsters won't talk, except priests (who wake up) */
    if (!mtmp->mcanmove && !mtmp->ispriest) {
		/* If it is unseen, the player can't tell the difference between
		   not noticing him and just not existing, so skip the message. */
		if (canspotmon(mtmp))
			pline("%s seems not to notice you.", Monnam(mtmp));
		return MOVE_INSTANT;
    }
    if (is_deaf(mtmp) && !mtmp->mcansee) {
		/* If it is unseen, the player can't tell the difference between
		   not noticing him and just not existing, so skip the message. */
		if (canspotmon(mtmp))
			pline("%s seems not to notice you.", Monnam(mtmp));
		return MOVE_INSTANT;
    }
    /* sleeping monsters won't talk unless they wake up, except priests (who wake up) */
	if (mtmp->msleeping){
		if(mtmp->ispriest || !rn2(2)) {
			pline("%s wakes from %s slumber.", Monnam(mtmp), mhis(mtmp));
			mtmp->msleeping = 0;
		}
		else {
			pline("%s stirs in %s slumber, but doesn't wake up.", Monnam(mtmp), mhis(mtmp));
			return MOVE_STANDARD;
		}
	}


    /* if this monster is waiting for something, prod it into action */
    mtmp->mstrategy &= ~STRAT_WAITMASK;

    /* laughing monsters can't talk */
    if (!mtmp->mnotlaugh) {
		if (!is_silent_mon(mtmp)) pline("%s laughs hysterically", Monnam(mtmp));
		return MOVE_INSTANT;
    }
	
    if (mtmp->mtame && mtmp->mnotlaugh && mtmp->meating) {
		if (!canspotmon(mtmp))
			map_invisible(mtmp->mx, mtmp->my);
		pline("%s is eating noisily. Looks like it will take %d turns to finish.", Monnam(mtmp), mtmp->meating);
		return MOVE_INSTANT;
    }
	
	if(mtmp->mtyp == PM_NIGHTGAUNT && u.umonnum == PM_GHOUL){
		You("bark the secret passwords known to ghouls.");
		mtmp->mpeaceful = 1;
		mtmp = tamedog(mtmp, (struct obj *)0);
		return MOVE_STANDARD;
	}
	if(is_undead(mtmp->data) && u.specialSealsActive&SEAL_ACERERAK && u.ulevel > mtmp->m_lev){
		You("order the lesser dead to stand at ease.");
		mtmp->mpeaceful = 1;
		mtmp->mhp = mtmp->mhpmax;
		return MOVE_STANDARD;
	}
	if(mtmp->mtyp == PM_LADY_CONSTANCE && !mtmp->mtame && mtmp->mpeaceful && Role_if(PM_MADMAN) && u.uevent.qcompleted){
		verbalize("Let's get out of here!");
		mtmp->mpeaceful = 1;
		mtmp = tamedog(mtmp, (struct obj *)0);
		if(mtmp && mtmp->mtame && get_mx(mtmp, MX_EDOG)){
			EDOG(mtmp)->loyal = TRUE;
			EDOG(mtmp)->waspeaceful = TRUE;
			mtmp->mpeacetime = 0;
		}
		return MOVE_STANDARD;
	}
    /* That is IT. EVERYBODY OUT. You are DEAD SERIOUS. */
    if (mtmp->mtyp == PM_URANIUM_IMP) {
		monflee(mtmp, rn1(20,10), TRUE, FALSE);
    }

    if (Role_if(PM_CONVICT) && is_rat(mtmp->data) && !mtmp->mpeaceful &&
     !mtmp->mtame) {
        You("attempt to soothe the %s with chittering sounds.",
         l_monnam(mtmp));
        if (rnl(100) < 20) {
            (void) tamedog(mtmp, (struct obj *) 0);
        } else {
            if (rnl(100) >= 90) {
                pline("%s unfortunately ignores your overtures.",
                 Monnam(mtmp));
                return MOVE_STANDARD;
            }
            mtmp->mpeaceful = 1;
            set_malign(mtmp);
        }
        return MOVE_STANDARD;
    }

    return domonnoise(mtmp, TRUE) ? MOVE_STANDARD : MOVE_INSTANT;
}

//definition of externs in you.h
long wis_spirits = SEAL_AMON|SEAL_BUER|SEAL_MOTHER|SEAL_SIMURGH;
long int_spirits = SEAL_ANDREALPHUS|SEAL_NABERIUS|SEAL_OSE|SEAL_PAIMON;

//definition of an extern in you.h
//A bag, a silver key, a gold ring, (a pair of dice), a (copper) coin, a dagger, an apple, a scroll, (a comb), a whistle, a mirror, an egg, a potion, a dead spider, (an oak leaf), a dead human (skull and arm bone), (a lock), (a closed black book) a spellbook, a bell, (a (live?) dove), a set of lockpicks, or a live? sewer rat (mouse). The items are consumed.
char *andromaliusItems[18] = {
/*00*/	"a sack",
/*01*/	"a silver key",
/*02*/	"a gold ring",
/*03*/	"a coin",
/*04*/	"a dagger",
/*05*/	"an apple",
/*06*/	"a scroll",
/*07*/	"a whistle",
/*08*/	"a mirror",
/*09*/	"an egg",
/*00*/	"a potion",
/*11*/	"a dead spider",
/*12*/	"a skull",
/*13*/	"an arm bone",
/*14*/	"a spellbook",
/*15*/	"a bell",
/*16*/	"a set of lockpicks",
/*17*/	"a live sewer rat"
};
static const int androCorpses[] = {
	PM_ELF,
	PM_DWARF,
	PM_GNOME,
	PM_ORC,
	PM_HUMAN,
	PM_HOBBIT,
	PM_DEEP_ONE,
	PM_MONKEY,
	PM_APE,
	PM_YETI,
	PM_CARNIVOROUS_APE,
	PM_SASQUATCH
};

int
binder_nearvoid_slots()
{
	int numSlots;
	if (Role_if(PM_EXILE)){
		if (u.ulevel <= 2) numSlots = 1;
		else if (u.ulevel <= 9) numSlots = 2;
		else if (u.ulevel <= 17) numSlots = 3;
		else if (u.ulevel <= 25) numSlots = 4;
		else numSlots = 5;
	}
	else {
		numSlots = 1;
	}
	return numSlots;
}

int
dobinding(tx,ty)
int tx,ty;
{
	struct engr *ep = get_head_engr();
	int numSlots = binder_nearvoid_slots();
	int i;
	int bindingPeriod = 5000;
	for(;ep;ep=ep->nxt_engr)
		if(ep->engr_x==tx && ep->engr_y==ty)
			break;//else continue
	if(!(ep)) return MOVE_CANCELLED; //no engraving found
	if(ep->halu_ward || ep->ward_id < FIRST_SEAL) return MOVE_CANCELLED;
	else if(ep->complete_wards < 1){
		pline("The seal has been damaged.");
		return MOVE_INSTANT;
	// } else if(ep->engr_time+5 < moves){
		// pline("The seal is too old.");
		// return 0;
	}

    boolean hasSealofSpirits = FALSE;
    {
      struct obj *otmp;
      for(otmp = level.objects[tx][ty]; otmp; otmp = otmp->nexthere){
        if(otmp->oartifact && otmp->oartifact == ART_SEAL_OF_THE_SPIRITS)
          hasSealofSpirits = TRUE;
      }
    }
	
	if(m_at(tx,ty) && (ep->ward_id != ANDROMALIUS || m_at(tx,ty)->mtyp != PM_SEWER_RAT)) return MOVE_CANCELLED;
	
	if(u.veil){
		You("feel reality threatening to slip away!");
		if (yn("Are you sure you want proceed with the ritual?") != 'y'){
			return MOVE_CANCELLED;
		}
		else pline("So be it.");
		u.veil = FALSE;
		change_uinsight(1);
	}
	switch(ep->ward_id){
	case AHAZU:{
		if(u.sealTimeout[AHAZU-FIRST_SEAL] < moves){
			struct trap *t=t_at(tx,ty);
			//Ahazu requires that his seal be drawn in a pit.
			if(t && t->ttyp == PIT){
				pline("The walls of the pit are lifted swiftly away, revealing a vast starry expanse beneath the world.");
				if(u.sealCounts < numSlots){
					pline("A voice whispers from below:");
					pline("\"All shall feed the shattered night.\"");
					bindspirit(ep->ward_id);
					u.sealTimeout[AHAZU-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					pline("A voice whispers from below:");
					pline("\"All shall feed the shattered night.\"");
					uwep->ovar1_seals |= SEAL_AHAZU;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_AHAZU;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_AHAZU;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[AHAZU-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					pline("A voice whispers from below, but you don't catch what it says.");
					// u.sealTimeout[AHAZU-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				pline("Thoughts of falling and of narrow skies come unbidden into your mind.");
				// u.sealTimeout[AHAZU-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case AMON:{
		if(u.sealTimeout[AMON-FIRST_SEAL] < moves){
			int curx, cury;
			char altarfound=0;
			//Amon can't be invoked on levels with altars, and in fact doing so causes imediate level loss, as for a broken taboo.
			for(curx=1;curx < COLNO;curx++){
				for(cury=1;cury < ROWNO;cury++){
					if(IS_ALTAR(levl[curx][cury].typ)) { altarfound = 1; break; }
				}
				if (altarfound == 1) break;
			}//end search
			
			if(!altarfound){
				pline("A golden flame roars suddenly to life within the seal, throwning the world into a stark relief of hard-edged shadows and brilliant light.");
				if(u.sealCounts < numSlots){
					pline("No sooner are the shadows born than they rise up against their creator, smothering the flame under a tide of darkness.");
					pline("Even as it dies, a voice speaks from the blood-red flame:");
					pline("\"Cursed are you who calls me forth. I damn you to bear my sign and my flames, alone in this world of darkness!\"");
					bindspirit(ep->ward_id);
					u.sealTimeout[AMON-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					pline("No sooner are the shadows born than they rise up against their creator, smothering the flame under a tide of darkness.");
					pline("Even as it dies, a voice speaks from the blood-red flame:");
					pline("\"Cursed are you who calls me forth. I damn you to bear my flames, alone in this world of darkness!\"");
					uwep->ovar1_seals |= SEAL_AMON;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_AMON;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_AMON;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[AMON-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					pline("No sooner are the shadows born than they rise up against their creator, smothering the flame under a tide of darkness.");
					// u.sealTimeout[AMON-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			}
			else{
				Your("mind's eye is blinded by a flame blasting through an altar.");
				losexp("shredding of the soul",TRUE,TRUE,TRUE);
				if(in_rooms(curx, cury, TEMPLE)){
//					struct monst *priest = findpriest(roomno);
					//invoking Amon inside a temple angers the resident deity
					altar_wrath(curx, cury);
					angrygods(god_at_altar(curx, cury));
				}
				u.sealTimeout[AMON-FIRST_SEAL] = moves + bindingPeriod; // invoking amon on a level with an altar still triggers the binding period.
			}
		} else pline("You can't feel the spirit.");
	}break;
	case ANDREALPHUS:{
		if(u.sealTimeout[ANDREALPHUS-FIRST_SEAL] < moves){
			//Andrealphus requires that his seal be drawn in a corner.
			if(isok(tx+(tx-u.ux), ty+(ty-u.uy)) && IS_CORNER(levl[tx+(tx-u.ux)][ty+(ty-u.uy)].typ) && 
				IS_WALL(levl[tx+(tx-u.ux)][ty].typ) && IS_WALL(levl[tx][ty+(ty-u.uy)].typ)
			){
				Your("perspective shifts, and the walls before you take on new depth.");
				pline("The dim dungeon light refracts oddly, casting the alien figure before you in rainbow hues.");
				if(u.sealCounts < numSlots){
					pline("\"I am Andrealphus, born of angles. In this soft world of curves, I alone am straight and true.\"");
					pline("\"Though born of curves, by my square you shall rectify the world.\"");
					bindspirit(ep->ward_id);
					u.sealTimeout[ANDREALPHUS-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					pline("\"I am Andrealphus, born of angles. In this soft world of curves, I alone am straight and true.\"");
					pline("\"Though your instrument is born of the point, by my square it shall rectify the world.\"");
					uwep->ovar1_seals |= SEAL_ANDREALPHUS;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_ANDREALPHUS;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_ANDREALPHUS;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[ANDREALPHUS-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					pline("\"I am Andrealphus, born of angles. In this soft world of curves, I alone am straight and true.\"");
					pline("\"You, born of dishonest curves, are unworthy of my measure.\"");
					// u.sealTimeout[ANDREALPHUS-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				pline("Thoughts intersecting lines rise to the forefront of your mind.");
				// u.sealTimeout[ANDREALPHUS-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case ANDROMALIUS:{ /*UNFINISHED*/
		//Seal must be drawn around any two of a bag, a silver key, a gold ring, (a pair of dice), a (copper) coin, a dagger, an apple, a scroll, (a comb), a whistle, a mirror, an egg, a potion, a dead spider, (an oak leaf), a dead human (skull and arm bone), (a lock), (a closed black book) a spellbook, a bell, (a (live?) dove), a set of lockpicks, or a live? sewer rat (mouse). The items are consumed.
		if(u.sealTimeout[ANDROMALIUS-FIRST_SEAL] < moves){
			struct obj *o1 = 0, *o2 = 0, *otmp;
			struct monst *rat = 0;
			int count = 0;
			int t1, t2;
			
			rat = m_at(tx,ty);
			if(rat && rat->mtyp == PM_SEWER_RAT) t2=17;
			else rat = 0;
			
			for(otmp = level.objects[tx][ty]; otmp; otmp = otmp->nexthere)
				if(!otmp->oartifact){
					if(!o1){
						if(otmp->otyp == SACK){ o1 = otmp; t1 = 0;}
						else if(otmp->otyp == UNIVERSAL_KEY){ o1 = otmp; t1 = 1;}
						else if(otmp->oclass == RING_CLASS && otmp->otyp == find_gold_ring()){ o1 = otmp; t1 = 2;}
						else if(otmp->oclass == COIN_CLASS){ o1 = otmp; t1 = 3;}
						else if(otmp->otyp == DAGGER){ o1 = otmp; t1 = 4;}
						else if(otmp->otyp == APPLE){ o1 = otmp; t1 = 5;}
						else if(otmp->oclass == SCROLL_CLASS){ o1 = otmp; t1 = 6;}
						else if(otmp->otyp == WHISTLE){ o1 = otmp; t1 = 7;}
						else if(otmp->otyp == MIRROR){ o1 = otmp; t1 = 8;}
						else if(otmp->otyp == EGG){ o1 = otmp; t1 = 9;}
						else if(otmp->oclass == POTION_CLASS){ o1 = otmp; t1 = 10;}
						else if(otmp->otyp == CORPSE && otmp->corpsenm==PM_CAVE_SPIDER){ o1 = otmp; t1 = 11;}
						else if(otmp->otyp == CORPSE && your_race(&mons[otmp->corpsenm])){ o1 = otmp; t1 = 12;}
						else if(otmp->otyp == CLOCKWORK_COMPONENT && Race_if(PM_CLOCKWORK_AUTOMATON)){ o1 = otmp; t1 = 12;}
						else if(otmp->otyp == BROKEN_ANDROID && Race_if(PM_ANDROID)){ o1 = otmp; t1 = 12;}
						else if(otmp->otyp == BROKEN_GYNOID && Race_if(PM_ANDROID)){ o1 = otmp; t1 = 12;}
						else if(otmp->otyp == CORPSE && is_andromaliable(&mons[otmp->corpsenm]) ){ o1 = otmp; t1 = 13;}
						else if(otmp->oclass == SPBOOK_CLASS){ o1 = otmp; t1 = 14;}
						else if(otmp->otyp == BELL){ o1 = otmp; t1 = 15;}
						else if(otmp->otyp == LOCK_PICK){ o1 = otmp; t1 = 16;}
					}
					else if(!o2 && !rat){
						if(otmp->otyp == SACK && otmp->otyp != o1->otyp){ o2 = otmp; t2 = 0;}
						else if(otmp->otyp == UNIVERSAL_KEY && otmp->otyp != o1->otyp){ o2 = otmp; t2 = 1;}
						else if(otmp->oclass == RING_CLASS && otmp->otyp == find_gold_ring() && otmp->oclass != o1->oclass){ o2 = otmp; t2 = 2;}
						else if(otmp->oclass == COIN_CLASS && otmp->oclass != o1->oclass){ o2 = otmp; t2 = 3;}
						else if(otmp->otyp == DAGGER && otmp->otyp != o1->otyp){ o2 = otmp; t2 = 4;}
						else if(otmp->otyp == APPLE && otmp->otyp != o1->otyp){ o2 = otmp; t2 = 5;}
						else if(otmp->oclass == SCROLL_CLASS && otmp->oclass != o1->oclass){ o2 = otmp; t2 = 6;}
						else if(otmp->otyp == WHISTLE && otmp->otyp != o1->otyp){ o2 = otmp; t2 = 7;}
						else if(otmp->otyp == MIRROR && otmp->otyp != o1->otyp){ o2 = otmp; t2 = 8;}
						else if(otmp->otyp == EGG && otmp->otyp != o1->otyp){ o2 = otmp; t2 = 9;}
						else if(otmp->oclass == POTION_CLASS && otmp->oclass != o1->oclass){ o2 = otmp; t2 = 10;}
						else if(otmp->otyp == CORPSE && otmp->corpsenm==PM_CAVE_SPIDER && t1 != 11){ o2 = otmp; t2 = 11;}
						else if(otmp->otyp == CORPSE && your_race(&mons[otmp->corpsenm]) && t1 != 12 && otmp != o1){ o2 = otmp; t2 = 12;}
						else if(otmp->otyp == CLOCKWORK_COMPONENT && Race_if(PM_CLOCKWORK_AUTOMATON) && t1 != 12 && otmp != o1){ o2 = otmp; t2 = 12;}
						else if(otmp->otyp == BROKEN_ANDROID && Race_if(PM_ANDROID) && t1 != 12 && otmp != o1){ o2 = otmp; t2 = 12;}
						else if(otmp->otyp == BROKEN_GYNOID && Race_if(PM_ANDROID) && t1 != 12 && otmp != o1){ o2 = otmp; t2 = 12;}
						else if(otmp->otyp == CORPSE && is_andromaliable(&mons[otmp->corpsenm]) && t1 != 13 && otmp != o1){ o2 = otmp; t2 = 13;}
						else if(otmp->oclass == SPBOOK_CLASS && otmp->oclass != o1->oclass){ o2 = otmp; t2 = 14;}
						else if(otmp->otyp == BELL && otmp->otyp != o1->otyp){ o2 = otmp; t2 = 15;}
						else if(otmp->otyp == LOCK_PICK && otmp->otyp != o1->otyp){ o2 = otmp; t2 = 16;}
					}
					else break;
				}
			if((o2 || rat) && o1){
				int i1 = rn2(18), i2 = rn2(18), i3 = rn2(18);
				
				while(i1 == t1 || i1 == t2) i1 = rn2(18);
				while(i2 == t1 || i2 == t2 || i2 == i1) i2 = rn2(18);
				while(i3 == t1 || i3 == t2 || i3 == i1 || i3 == i2) i3 = rn2(18);
				
				pline("Gloved hands reach down and pick up %s and %s from the confines of the seal.", andromaliusItems[t1], andromaliusItems[t2]);
				pline("The hands begin to juggle. They move faster and faster, adding new objects as they go.");
				pline("You spot %s and %s before losing track of the individual objects.",andromaliusItems[i1],andromaliusItems[i2]);
				if(u.sealCounts < numSlots){
					pline("Suddenly, the hands toss one of the whirling objects to you.");
					/*make object here*/
					switch(i3){
						case 0:
							otmp = mksobj(SACK, NO_MKOBJ_FLAGS);
							otmp->blessed = FALSE;
							otmp->cursed = FALSE;
							hold_another_object(otmp, "You drop %s!",
								doname(otmp), (const char *)0);
						break;
						case 1:
							otmp = mksobj(UNIVERSAL_KEY, NO_MKOBJ_FLAGS);
							otmp->blessed = FALSE;
							otmp->cursed = FALSE;
							hold_another_object(otmp, "You drop %s!",
								doname(otmp), (const char *)0);
						break;
						case 2:
							otmp = mksobj(find_gold_ring(), NO_MKOBJ_FLAGS);
							otmp->blessed = FALSE;
							otmp->cursed = TRUE;
							hold_another_object(otmp, "You drop %s!",
								doname(otmp), (const char *)0);
						break;
						case 3://Coin
							otmp = mkobj(COIN_CLASS, FALSE);
							otmp->quan = 1;
							otmp->owt = weight(otmp);
							hold_another_object(otmp, "You drop %s!",
								doname(otmp), (const char *)0);
						break;
						case 4:
							otmp = mksobj(DAGGER, NO_MKOBJ_FLAGS);
							otmp->blessed = FALSE;
							otmp->cursed = FALSE;
							hold_another_object(otmp, "You drop %s!",
								doname(otmp), (const char *)0);
						break;
						case 5:
							otmp = mksobj(APPLE, NO_MKOBJ_FLAGS);
							otmp->blessed = FALSE;
							otmp->cursed = FALSE;
							hold_another_object(otmp, "You drop %s!",
								doname(otmp), (const char *)0);
						break;
						case 6:
							otmp = mkobj(SCROLL_CLASS, FALSE);
							otmp->blessed = FALSE;
							otmp->cursed = FALSE;
							hold_another_object(otmp, "You drop %s!",
								doname(otmp), (const char *)0);
						break;
						case 7:
							otmp = mksobj(WHISTLE, NO_MKOBJ_FLAGS);
							otmp->blessed = FALSE;
							otmp->cursed = FALSE;
							hold_another_object(otmp, "You drop %s!",
								doname(otmp), (const char *)0);
						break;
						case 8:
							otmp = mksobj(MIRROR, NO_MKOBJ_FLAGS);
							otmp->blessed = FALSE;
							otmp->cursed = FALSE;
							hold_another_object(otmp, "You drop %s!",
								doname(otmp), (const char *)0);
						break;
						case 9:
							otmp = mksobj(EGG, NO_MKOBJ_FLAGS);
							otmp->blessed = FALSE;
							otmp->cursed = FALSE;
							hold_another_object(otmp, "You drop %s!",
								doname(otmp), (const char *)0);
						break;
						case 10:
							otmp = mkobj(POTION_CLASS, FALSE);
							otmp->blessed = FALSE;
							otmp->cursed = FALSE;
							hold_another_object(otmp, "You drop %s!",
								doname(otmp), (const char *)0);
						break;
						case 11:
							otmp = mksobj(CORPSE, NO_MKOBJ_FLAGS);
							otmp->corpsenm = PM_CAVE_SPIDER;
							otmp->owt = weight(otmp);
							hold_another_object(otmp, "You drop %s!",
								doname(otmp), (const char *)0);
						break;
						case 12:
							//Note: "Skull"
							//Androids need a broken android, clockworks need a mechanism, Vampires need a human
							if(Race_if(PM_ANDROID)){
								otmp = mksobj(flags.female ? BROKEN_ANDROID : BROKEN_GYNOID, NO_MKOBJ_FLAGS);
								otmp->owt = weight(otmp);
							}
							else if(Race_if(PM_CLOCKWORK_AUTOMATON)){
								otmp = mksobj(CLOCKWORK_COMPONENT, NO_MKOBJ_FLAGS);
								otmp->owt = weight(otmp);
							}
							else {
								otmp = mksobj(CORPSE, NO_MKOBJ_FLAGS);
								otmp->corpsenm = (Race_if(PM_VAMPIRE) || Race_if(PM_INCANTIFIER)) ? PM_HUMAN : urace.malenum;
								otmp->oeaten = mons[otmp->corpsenm].cnutrit;
								consume_oeaten(otmp, 1);
								otmp->owt = weight(otmp);
							}
							hold_another_object(otmp, "You drop %s!",
								doname(otmp), (const char *)0);
						break;
						case 13:
							//Note: "Arm bone"
							otmp = mksobj(CORPSE, NO_MKOBJ_FLAGS);
							otmp->corpsenm = androCorpses[rn2(SIZE(androCorpses))];
							otmp->oeaten = mons[otmp->corpsenm].cnutrit;
							consume_oeaten(otmp, 1);
							otmp->owt = weight(otmp);
							hold_another_object(otmp, "You drop %s!",
								doname(otmp), (const char *)0);
						break;
						case 14:
							otmp = mkobj(SPBOOK_CLASS, FALSE);
							otmp->blessed = FALSE;
							otmp->cursed = TRUE;
							hold_another_object(otmp, "You drop %s!",
								doname(otmp), (const char *)0);
						break;
						case 15:
							otmp = mksobj(BELL, NO_MKOBJ_FLAGS);
							otmp->blessed = FALSE;
							otmp->cursed = FALSE;
							hold_another_object(otmp, "You drop %s!",
								doname(otmp), (const char *)0);
						break;
						case 16:
							otmp = mksobj(LOCK_PICK, NO_MKOBJ_FLAGS);
							otmp->blessed = FALSE;
							otmp->cursed = FALSE;
							hold_another_object(otmp, "You drop %s!",
								doname(otmp), (const char *)0);
						break;
						case 17:
							rat = makemon(&mons[PM_SEWER_RAT], u.ux, u.uy, MM_EDOG|MM_ADJACENTOK|NO_MINVENT|MM_NOCOUNTBIRTH);
							if(rat){
								initedog(rat);
								if(rat->mtame) EDOG(rat)->loyal = TRUE;
								rat->mtame = 10;
								rat->mpeaceful = 1;
								pline("A startled-looking rat lands in your %s, then leaps to the floor.",makeplural(body_part(HAND)));
								rat = 0;
							}
						break;
					}
					pline("When your attention returns to the seal, the hands have gone.");
					bindspirit(ep->ward_id);
					u.sealTimeout[ANDROMALIUS-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					pline("Suddenly, the hands toss one of the whirling objects to you.");
					pline("It's the Pen of the Void. You didn't notice it had been taken!");
					uwep->ovar1_seals |= SEAL_ANDROMALIUS;
					if(!u.spiritTineA){
						u.spiritTineA = SEAL_ANDROMALIUS;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_ANDROMALIUS;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[ANDROMALIUS-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					pline("Suddenly, the hands toss one of the whirling objects at you.");
					You("see %s pass far over your %s, out of reach.", andromaliusItems[i3], body_part(HEAD));
					pline("When your attention returns to the seal, the hands have gone.");
					// u.sealTimeout[ANDROMALIUS-FIRST_SEAL] = moves + bindingPeriod/10;
				}
				if(o1){
					if(o1->quan > 1) o1->quan--; 
					else{
						obj_extract_self(o1);
						obfree(o1, (struct obj *)0);
						o1 = (struct obj *) 0;
						// useup(o1);
					}
				}
				if(rat) mongone(rat);
				if(o2){
					if(o2->quan > 1) o2->quan--; 
					else{
						obj_extract_self(o2);
						obfree(o2, (struct obj *)0);
						o2 = (struct obj *) 0;
						// useup(o2);
					}
				}
				newsym(tx,ty);
			} else{
				int i1 = rn2(18), i2 = rn2(18);				
				while(i1 == i2) i2 = rn2(18);
				pline("The image of a pair of gloved hands comes unbidden to the forefront of your mind.");
				pline("With your mind's eye, you watch as they make a show of prestidigitation,");
				pline("palming and unpalming %s. Suddenly, they throw %s at your face!", andromaliusItems[i1], andromaliusItems[i2]);
				You("come out of your revere with a start.");
				// u.sealTimeout[ANDROMALIUS-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case ASTAROTH:{
		if(u.sealTimeout[ASTAROTH-FIRST_SEAL] < moves){
			struct obj *o = 0, *otmp;
			char prefix[32]; //thoroughly 11, corroded 9
			boolean iscrys;
			prefix[0] = '\0';
			for(otmp = level.objects[tx][ty]; otmp; otmp = otmp->nexthere){
				if(otmp->spe < 0 || otmp->oeroded || otmp->oeroded2){
					o = otmp;
			break;
				}
			}
			//Astaroth requires that his seal be drawn on a square with a damaged item.
			if(o && u.sealCounts < numSlots){
				iscrys = (o->otyp == CRYSKNIFE);
				if (o->oeroded && !iscrys) {
					switch (o->oeroded) {
						case 2:	Strcat(prefix, "very "); break;
						case 3:	Strcat(prefix, "thoroughly "); break;
					}			
					Strcat(prefix, is_rustprone(o) ? "rusty " : "burnt ");
				}
				if (o->oeroded2 && !iscrys) {
					switch (o->oeroded2) {
						case 2:	Strcat(prefix, "very "); break;
						case 3:	Strcat(prefix, "thoroughly "); break;
					}			
					Strcat(prefix, is_corrodeable(o) ? "corroded " :
						"rotted ");
				}
				pline("A hand of worn and broken clockwork on a rusted metal arm reaches into the seal.");
				pline("The hand gently touches the %s%s, then rests on the seal's surface as its unseen owner shifts his weight onto that arm.", prefix, xname(o));
				pline("There is the sound of shrieking metal, and a cracked porcelain face swings into view on a metallic armature.");
				pline("A voice speaks to you, as the immobile white face weeps tears of black oil onto the %s.", surface(tx,ty));
				pline("*I am Astaroth, the Clockmaker. You shall be my instrument, to repair this broken world.*");
				bindspirit(ep->ward_id);
				if(o->spe<0) o->spe=0;
				if(o->oeroded) o->oeroded=0;
				if(o->oeroded2) o->oeroded2=0;
				u.sealTimeout[ASTAROTH-FIRST_SEAL] = moves + bindingPeriod;
			}
			else if(uwep && (uwep->spe<0 || uwep->oeroded || uwep->oeroded2) && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
				pline("A hand of worn and broken clockwork on a rusted metal arm reaches into the seal.");
				pline("The hand slowly stretches out towards you, then rests on the seal's surface as its unseen owner shifts his weight onto that arm.");
				pline("There is the sound of shrieking metal, and a cracked porcelain face swings into view on a metallic armature.");
				pline("A voice speaks to you, as the immobile white face studies you and weeps tears of black oil.");
				pline("*I am Astaroth, the Clockmaker. You shall hold my instrument, to repair this broken world.*");
				uwep->ovar1_seals |= SEAL_ASTAROTH;
				if(!u.spiritTineA){
					u.spiritTineA = SEAL_ASTAROTH;
					u.spiritTineTA= moves + bindingPeriod;
				}
				else{
					u.spiritTineB = SEAL_ASTAROTH;
					u.spiritTineTB= moves + bindingPeriod;
				}
				pline("The hand catches a teardrop and anoints the Pen of the Void with the glistening oil.");
				if(uwep->spe<0) uwep->spe=0;
				if(uwep->oeroded) uwep->oeroded=0;
				if(uwep->oeroded2) uwep->oeroded2=0;
				u.sealTimeout[ASTAROTH-FIRST_SEAL] = moves + bindingPeriod;
			}
			else if(o || (uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (uwep->spe<0 || uwep->oeroded || uwep->oeroded2))){
				pline("Black oil falls like teardrops into the seal.");
				if(o){
					if(o->spe<0) o->spe++;
					if(o->oeroded) o->oeroded--;
					if(o->oeroded2) o->oeroded2--;
					pline("But nothing else occurs.");
				}
				else{
					if(uwep->spe<0) uwep->spe++;
					if(uwep->oeroded) uwep->oeroded--;
					if(uwep->oeroded2) uwep->oeroded2--;
					pline("The Pen of the Void drips black oil, as if in sympathy.");
				}
				// u.sealTimeout[ASTAROTH-FIRST_SEAL] = moves + bindingPeriod/10;
			} else {
				o = (struct obj *) 0;
				for(otmp = level.objects[tx][ty]; otmp; otmp = otmp->nexthere){
					if(otmp->otyp == BELL_OF_OPENING && (otmp->spe == 0 || Role_if(PM_EXILE))){
						o = otmp;
				break;
					}
				}
				if(o){
					pline("A hand of worn and broken clockwork on a rusted metal arm reaches into the seal and rings the bell.");
					use_bell(&o,TRUE);
					u.sealTimeout[ASTAROTH-FIRST_SEAL] = moves + bindingPeriod/10;
					pline("*Sometimes, what was broken cannot be restored.*");
				} else {
					pline("You think of all the loyal items used up and thrown away each day, and shed a tear.");
					// u.sealTimeout[ASTAROTH-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			}
		} else pline("You can't feel the spirit.");
	}break;
	case BALAM:{
		if(u.sealTimeout[BALAM-FIRST_SEAL] < moves){
			//Balam requires that her seal be drawn on an icy square.
			if(levl[tx][ty].typ == ICE && uwep){
				You("stab your weapon down into the ice, cracking it.");
				if(u.sealCounts < numSlots){
					if(!Blind){
						pline("A woman's scream echos through your mind as the cracks form a vaguely humanoid outline on the ice.");
						pline("A voice sobs in your ear:");
					} else {
						pline("A woman's scream echos through your mind, then a voice sobs in your ear:");
					}
					pline("\"I am Balam, offered up as the last sacrifice; condemned to bleed until the end of all suffering.\"");
					pline("\"In your name was this done, therefore you shall bear my stigmata and share my suffering.\"");
					bindspirit(ep->ward_id);
					u.sealTimeout[BALAM-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					if(!Blind){
						pline("A woman's scream echos through your mind as the cracks form a vaguely humanoid outline on the ice.");
						pline("A voice sobs from under the ice:");
					} else {
						pline("A woman's scream echos through your mind, then a voice sobs from under the ice:");
					}
					pline("\"I am Balam, offered up as the last sacrifice; condemned to bleed until the end of all suffering.\"");
					pline("\"By your hand was this done, therefore you shall be stained by my blood.\"");
					uwep->ovar1_seals |= SEAL_BALAM;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_BALAM;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_BALAM;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[BALAM-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					if(!Blind){
						pline("A woman's scream drifts through your mind, and the cracks describe a vaguely humanoid outline on the ice.");
						pline("But nothing else occurs....");
					} else {
						pline("A woman's scream drifts through your mind, but nothing else occurs....");
					}
					// u.sealTimeout[BALAM-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				if(!uwep) Your("weapon-hand itches.");
				You("shiver violently.");
				// u.sealTimeout[BALAM-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case BERITH:{
		if(u.sealTimeout[BERITH-FIRST_SEAL] < moves){
			struct obj *o = 0, *otmp;
			//Berith requires that his seal be drawn around a set of riding gloves, riding boots, a saddle, a saber, a longsword, a bow, or a lance.
			for(otmp = level.objects[tx][ty]; otmp; otmp = otmp->nexthere){
				if(is_berithable(otmp)){
					o = otmp;
			break;
				}
			}
			//Berith also allows the summoner to wear a blessed silver ring on his or her left hand.
			if (o || (uleft && uleft->otyp == find_silver_ring() && uleft->blessed)){
				if(u.sealCounts < numSlots){
					if(!Blind){
						pline("Gold rains down within the circumference of the seal, melting slowly to blood where it lands.");
						pline("A figure takes form within the showering gold, staring down at you from a crimson horse.");
						pline("His crown is gold, and his clothes are red like blood.");
					}
					pline("\"I am Berith, %s.",rn2(2) ? "war-leader of the forgotten" : "god of the covenant of blood");
					pline("I anoint you in Blood and Gold, that bloodshed and riches shall follow in your wake.");
					pline("That is my covenant, my blessing, and my curse.\"");
					bindspirit(ep->ward_id);
					u.sealTimeout[BERITH-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					if(!Blind){
						pline("Gold rains down within the circumference of the seal, melting slowly to blood where it lands.");
						pline("A figure takes form within the showering gold, staring down at you from a crimson horse.");
						pline("His crown is gold, and his clothes are red like blood.");
					}
					pline("\"I am Berith, %s.",rn2(2) ? "war-leader of the forgotten" : "god of the covenant of blood");
					pline("I anoint your blade with Blood, for blood calls to blood.");
					pline("That is the covenant and curse of Berith.\"");
					uwep->ovar1_seals |= SEAL_BERITH;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_BERITH;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_BERITH;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[BERITH-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					You("think you can hear faint hoofbeats from within the seal.");
					pline("But they fade away before you can be sure.");
					// u.sealTimeout[BERITH-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				You("think of cavalry and silver rings.");
				// u.sealTimeout[BERITH-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case BUER:{
		//Buer's seal may be drawn anywhere.
		if(u.sealTimeout[BUER-FIRST_SEAL] < moves){
			pline("You hear hooved footfalls approaching quickly, though you can't make out from what direction.");
			pline("They set an odd tempo; very regular and faster by far than any animal of four legs could comfortably keep.");
			if(!Blind){
				pline("The footfalls reach a crescendo, and an odd creature rolls into the seal in front of you.");
				pline("The creature's five legs are arranged in a star pattern, and to move it rolls from foot to foot.");
				pline("At the center of the wheel is a lion's head, complete with a glorious mane.");
			}
			pline("The creature speaks to you; and it's voice, though deep, is clearly that of a woman.");
			pline("\"I am Buer, %s, %s to %s.", buerTitles[rn2(SIZE(buerTitles))], buerSetOne[rn2(SIZE(buerSetOne))], buerSetTwo[rn2(SIZE(buerSetTwo))]);
			if(u.sealCounts < numSlots){
				pline("Will you walk with me?\"");
				bindspirit(ep->ward_id);
				u.sealTimeout[BUER-FIRST_SEAL] = moves + bindingPeriod;
			}
			else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
				pline("I will walk beside you.\"");
				uwep->ovar1_seals |= SEAL_BUER;
				if(!u.spiritTineA){
					u.spiritTineA = SEAL_BUER;
					u.spiritTineTA= moves + bindingPeriod;
				}
				else{
					u.spiritTineB = SEAL_BUER;
					u.spiritTineTB= moves + bindingPeriod;
				}
				u.sealTimeout[BUER-FIRST_SEAL] = moves + bindingPeriod;
			}
			else{
				pline("I wish you well as you walk your path.\"");
				healup(d(5,spiritDsize()), 0, TRUE, TRUE);
				// u.sealTimeout[BUER-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case CHUPOCLOPS:{
		if(u.sealTimeout[CHUPOCLOPS-FIRST_SEAL] < moves){
			struct obj *o = 0, *otmp;
			for(otmp = level.objects[tx][ty]; otmp; otmp = otmp->nexthere){
				if(is_chupodible(otmp)){
					o = otmp;
			break;
				}
			}
			//Chupoclops requires that her seal be drawn around a humanoid corpse or grave.
			if(o || IS_GRAVE(levl[tx][ty].typ)){
				// pline("The %s within the seal begins to twitch and shake.");
				// Your("consciousness expands, and you sense great currents of despair and mortality that wrap the world like silken threads.");
				// pline("The %s falls still, and you know you're in the presence of the Spider.");
				pline("The great sweep of lives and civilizations seems represented in microcosm by the scene within the seal's boundaries.");
				You("sense the twin threads of mortality and despair,");
				pline("that wrap this world and its inhabitants like silken bindings, and, suddenly,");
				pline("you know you are in the presence of the Spider.");
				if(u.sealCounts < numSlots){
					pline("She wraps you tight in her bitter cords and sends you forth, bait within her web.");
					bindspirit(ep->ward_id);
					u.sealTimeout[CHUPOCLOPS-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					pline("She wraps your blade tight in her bitter cords, making it an anchor for her web.");
					uwep->ovar1_seals |= SEAL_CHUPOCLOPS;
					if(!u.spiritTineA){
						u.spiritTineA = SEAL_CHUPOCLOPS;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_CHUPOCLOPS;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[CHUPOCLOPS-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					pline("The Spider passes you over, and collects her prize.");
					
					if(o){
						useupf(o, 1L);
						newsym(tx,ty);
					} else{
						levl[tx][ty].typ = ROOM;
						digfarhole(TRUE,tx,ty, TRUE);
					}
					// u.sealTimeout[CHUPOCLOPS-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				pline("Thoughts of death and despair almost overcome you.");
				// u.sealTimeout[CHUPOCLOPS-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case DANTALION:{
		if(u.sealTimeout[DANTALION-FIRST_SEAL] < moves){
			//Spirit requires that his seal be drawn around a throne.
			if(IS_THRONE(levl[tx][ty].typ)){
				if(!Blind) {
					You("see a man with many countenances step out from behind the throne.");
					pline("Below his crown are many faces of %s,", DantalionRace(u.umonster)); /*  */
					pline("and as he nods and cranes his head, new faces are constantly revealed.");
				}
				pline("\"Tremble, for I am Dantalion, king over all the kings of %s\"",urace.coll);
				if(u.sealCounts < numSlots){
					if(!Blind) {
						pline("The staring faces seem vaguely familiar...");
						pline("With a start, you realize they remind you of yourself, and your family!");
					}
					pline("\"You, who bear my bloodline,\"");
					pline("\"go forth %swith my blessing.\"", flags.female ? "":"and rule ");
					bindspirit(ep->ward_id);
					u.sealTimeout[DANTALION-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					if(flags.initgend==1){ /*(female)*/
					}
					pline("\"You, who bear my sceptre,\"");
					pline("\"go forth, in my name!\"");
					uwep->ovar1_seals |= SEAL_DANTALION;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_DANTALION;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_DANTALION;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[DANTALION-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					if(!Blind) {
						pline("His myriad faces study you with disapproval, and he departs as suddenly as he arrived.");
					}
					// u.sealTimeout[DANTALION-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else {
				You_hear("royal trumpets.");
				// u.sealTimeout[DANTALION-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case SHIRO:{
		if(u.sealTimeout[SHIRO-FIRST_SEAL] < moves){
			int ttx, tty;
			boolean validLocation = TRUE;
			struct obj *otmp;
			for(ttx=tx-1; ttx<=tx+1; ttx++){
				for(tty=ty-1; tty<=ty+1; tty++){
					if(!isok(ttx,tty)) validLocation = FALSE;
					else if( !((otmp = level.objects[ttx][tty]) && otmp->otyp == ROCK) && !(ttx==tx && tty==ty)){
						validLocation = FALSE;
					}
				}
			}
			//Spirit requires that his seal be drawn in a ring of rocks.
			if(validLocation){
				pline("\"I'm shocked. So few ever speak to me, everyone ignores me and passes me by.\"");
				pline("\"It's 'cause I'm about as impressive as a stone, right?...I'm used to it, though.\"");
				if(u.sealCounts < numSlots){
					pline("\"You look like a pretty distinctive person.\"");
					pline("\"Let me follow you and practice standing out.\"");
					bindspirit(ep->ward_id);
					u.sealTimeout[SHIRO-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					pline("\"That looks like a pretty distinctive weapon.\"");
					pline("\"Let me follow you and see how you use it.\"");
					uwep->ovar1_seals |= SEAL_SHIRO;
					if(!u.spiritTineA){
						u.spiritTineA = SEAL_SHIRO;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_SHIRO;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[SHIRO-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					pline("\"Well, I'm certain a person as distinctive as you has better things to do than talk to me.\"");
					pline("\"I hope you visit again some time.\"");
					// u.sealTimeout[SHIRO-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				pline("For some reason you want to arrange rocks in a circle.");
				// u.sealTimeout[SHIRO-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case ECHIDNA:{
		if(u.sealTimeout[ECHIDNA-FIRST_SEAL] < moves){
			//NOT YET IMPLEMENTED: Spirit requires that her seal be drawn in a cave.
			if(In_cave(&u.uz)){
				if(!Blind){
					You("suddenly notice a monstrous nymph reclining in the center of the seal.");
					pline("She is half a fair woman, with glancing eyes and fair cheeks,");
					pline("and half again a terrible dragon, with great scaly wings and serpent's tails where legs should be.");
				}
				if(u.sealCounts < numSlots){
					pline("\"I am Echidna, %s.\"",echidnaTitles[rn2(SIZE(echidnaTitles))]);
					pline("\"Free me from this place, and I and my brood shall fight for your cause.\"");
					bindspirit(ep->ward_id);
					u.sealTimeout[ECHIDNA-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					pline("\"I am Echidna, %s.\"",echidnaTitles[rn2(SIZE(echidnaTitles))]);
					pline("\"Cut my bonds, and I shall lend my wrath to your cause.\"");
					uwep->ovar1_seals |= SEAL_ECHIDNA;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_ECHIDNA;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_ECHIDNA;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[ECHIDNA-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					if(!Blind){
						pline("She hisses at you, then slithers away.");
					} else {
						pline("Something hisses at you, then slithers away.");
					}
					// u.sealTimeout[ECHIDNA-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				You_hear("scales scraping against stone echo through a cave.");
				// u.sealTimeout[ECHIDNA-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case EDEN:{
		if(u.sealTimeout[EDEN-FIRST_SEAL] < moves){
			//Spirit requires that its seal be drawn by a fountain.
			if(IS_FOUNTAIN(levl[tx][ty].typ)){
				if(!Blind){
					pline("The water in the fountain begins to bubble.");
					pline("A dome of cerulean crystal emerges from the center of the fountain,");
					pline("the apex of a circular silver cathedral.");
					pline("As the water falls away, you see that the cathedral perches atop a stylized silver dragon.");
					pline("The needle-like tail of the dragon clears the fountain,");
					pline("and the gate in the dragon's jaws begins to open.");
				} else {
					You_hear("bubbling.");
				}
				if(u.sealCounts < numSlots){
					pline("Radiant light falls upon you,");
					pline("blinding you to what lies beyond.");
					bindspirit(ep->ward_id);
					u.sealTimeout[EDEN-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					pline("Radiant light falls on your weapon.");
					pline("The gates are angled such that you can't see past.");
					uwep->ovar1_seals |= SEAL_EDEN;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_EDEN;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_EDEN;
						u.spiritTineTB= moves + bindingPeriod;
					}
					if (uwep->opoisoned & OPOISON_SILVER)
						uwep->opoisoned &= ~OPOISON_SILVER;
					u.sealTimeout[EDEN-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					pline("But before they do, the whole construct jerks suddenly upwards,");
					pline("out of sight.");
					// u.sealTimeout[EDEN-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				You_hear("water splashing.");
				// u.sealTimeout[EDEN-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case ENKI:{
		if(u.sealTimeout[ENKI-FIRST_SEAL] < moves){
			boolean largeRoom = TRUE;
			int i, j;
			for(i = -2; i <= 2; i++){
				for(j=-2;j<= 2; j++){
					if(!isok(tx+i,ty+j) || 
						!IS_ROOM(levl[tx+i][ty+j].typ)
					) largeRoom = FALSE;
				}
			}
			//Spirit requires that his seal be drawn in a large open space.
			if(largeRoom){
				pline("Water bubbles up inside the seal,");
				pline("and a figure rises within it.");
				if(u.sealCounts < numSlots){
					pline("I am Enki, god of the first city.");
					pline("Bow to me, and I shall teach the arts of civilization.");
					bindspirit(ep->ward_id);
					u.sealTimeout[ENKI-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					pline("I am Enki, god of Eridu.");
					pline("Bow to me, and I shall lend aid from within the Abzu.");
					uwep->ovar1_seals |= SEAL_ENKI;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_ENKI;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_ENKI;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[ENKI-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					pline("But the figure is asleep,");
					pline("and sinks again without saying a word.");
					// u.sealTimeout[ENKI-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				You("dream of wide open spaces.");
				// u.sealTimeout[ENKI-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case EURYNOME:{
		if(u.sealTimeout[EURYNOME-FIRST_SEAL] < moves){
			//Spirit requires that her seal be drawn before some water.
			if(isok(tx+(tx-u.ux), ty+(ty-u.uy)) && 
				IS_PUDDLE_OR_POOL(levl[tx+(tx-u.ux)][ty+(ty-u.uy)].typ)
			){
				if(!Blind)
					You("see a figure dancing, far out upon the waters.");
				if(u.sealCounts < numSlots){
					if(!Blind){
						pline("She dances up to you and sweeps you up into her dance.");
						pline("She is beautiful, like nothing you have ever seen before.");
						pline("And yet, she reminds you of EVERYTHING you've ever seen before.");
					} else {
						pline("Something sweeps you into a dance.");
						pline("It feels odd to your touch, like touching everything in the world,");
						pline("and beyond it, all at once.");
					}
					bindspirit(ep->ward_id);
					u.sealTimeout[EURYNOME-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					if(!Blind){
						pline("She dances up to you and performs a sword dance with her claws.");
					}
					else You_hear("splashing.");
					uwep->ovar1_seals |= SEAL_EURYNOME;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_EURYNOME;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_EURYNOME;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[EURYNOME-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					if(!Blind) pline("but it pays you no heed.");
					// u.sealTimeout[EURYNOME-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				You("daydream of dancing across waves.");
				// u.sealTimeout[EURYNOME-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case EVE:{
		if(u.sealTimeout[EVE-FIRST_SEAL] < moves){
			//Spirit requires that her seal be drawn beside a tree.
			if(isok(tx+u.dx, ty+u.dy) && 
				IS_TREE(levl[tx+u.dx][ty+u.dy].typ) 
			){
				You("%s a woman within the seal.", Blind ? "sense" : "see");
				pline("She is both young and old at once.");
				pline("She looks hurt.");
				if(u.sealCounts < numSlots){
					You("help her to her feet.");
					pline("\"Shall we hunt together?\"");
					bindspirit(ep->ward_id);
					u.sealTimeout[EVE-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					You("try to help her, but she is unable to stand.");
					pline("She blesses your blade as thanks.");
					uwep->ovar1_seals |= SEAL_EVE;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_EVE;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_EVE;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[EVE-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					pline("In fact, you don't think she can move.");
					// u.sealTimeout[EVE-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				You_hear("wind in the trees.");
				// u.sealTimeout[EVE-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case FAFNIR:{
		if(u.sealTimeout[FAFNIR-FIRST_SEAL] < moves){
			boolean coins = FALSE;
			struct obj *otmp;
			for(otmp = level.objects[tx][ty]; otmp; otmp = otmp->nexthere){
				if(otmp->oclass == COIN_CLASS && otmp->quan >= 1000*u.ulevel){
					coins = TRUE;
			break;
				}
			}
			//Spirit requires that his seal be drawn in a vault, or on a pile of 1000xyour level coins.
			if(coins || (*in_rooms(tx,ty,VAULT) && u.uinvault)){
				if(!Blind) You("suddenly notice a dragon %s", coins ? "buired in the coins" : "in the room.");
				if(u.sealCounts < numSlots){
					if(!Blind) pline("The dragon lunges forwards to bite you.");
					else pline("something bites you!");
					Your("left finger stings!");
					bindspirit(ep->ward_id);
					u.sealTimeout[FAFNIR-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					if(!Blind) pline("The dragon tries to steal your weapon!");
					else pline("Something tries to steal your weapon!");
					You("fight it off.");
					uwep->ovar1_seals |= SEAL_FAFNIR;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_FAFNIR;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_FAFNIR;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[FAFNIR-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					pline("It roars at you to leave it alone.");
					// u.sealTimeout[FAFNIR-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				You_hear("the clink of coins.");
				// u.sealTimeout[FAFNIR-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case HUGINN_MUNINN:{
		if(u.sealTimeout[HUGINN_MUNINN-FIRST_SEAL] < moves){
			//Spirit places no restrictions on where their seal is drawn.
			You_hear("flapping wings.");
			if(!Blind) pline("A pair of ravens land in the seal.");
			if(u.sealCounts < numSlots){
				if(!Blind) pline("They hop up to your shoulders and begin to croak raucously in your ears.");
				else pline("A pair of large birds land on you and begin to croak raucously in your ears.");
				You("try to shoo them away, only to find that they have vanished.");
				bindspirit(ep->ward_id);
				if(ACURR(A_WIS)>ATTRMIN(A_WIS)){
					adjattrib(A_WIS, -1, FALSE);
					AMAX(A_WIS) -= 1;
				}
				if(ACURR(A_INT)>ATTRMIN(A_INT)){
					adjattrib(A_INT, -1, FALSE);
					AMAX(A_INT) -= 1;
				}
				u.sealTimeout[HUGINN_MUNINN-FIRST_SEAL] = moves + bindingPeriod;
			}
			else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
				if(!Blind) pline("They croak raucously at your weapon.");
				else You_hear("Raucous croaking.");
				uwep->ovar1_seals |= SEAL_HUGINN_MUNINN;
				if(!u.spiritTineA){ 
					u.spiritTineA = SEAL_HUGINN_MUNINN;
					u.spiritTineTA= moves + bindingPeriod;
				}
				else{
					u.spiritTineB = SEAL_HUGINN_MUNINN;
					u.spiritTineTB= moves + bindingPeriod;
				}
				if(ACURR(A_WIS)>ATTRMIN(A_WIS)){
					adjattrib(A_WIS, -1, FALSE);
				}
				if(ACURR(A_INT)>ATTRMIN(A_INT)){
					adjattrib(A_INT, -1, FALSE);
				}
				u.sealTimeout[HUGINN_MUNINN-FIRST_SEAL] = moves + bindingPeriod;
			}
			else{
				if(!Blind) pline("They stare at you for a moment, and then leave just as suddenly as they came.");
				// u.sealTimeout[HUGINN_MUNINN-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case IRIS:{
		if(u.sealTimeout[IRIS-FIRST_SEAL] < moves){
			//Spirit requires that her seal be drawn inside a stinking cloud.
			if(check_stinking_cloud_region((xchar)tx,(xchar)ty)
				|| check_solid_fog_region((xchar)tx,(xchar)ty)
				|| check_dust_cloud_region((xchar)tx,(xchar)ty)
				|| levl[tx][ty].typ == CLOUD
			){ 
				You("catch a glimpse of something moving in the%s cloud....", 
					check_solid_fog_region((xchar)tx,(xchar)ty) ? " fog" : 
					check_dust_cloud_region((xchar)tx,(xchar)ty) ? " dust" : 
					check_stinking_cloud_region((xchar)tx,(xchar)ty) ? " stinking" : ""
				);
				pline("But you can't see what it was.");
				if(u.sealCounts < numSlots){
					pline("Something jumps on you from behind!");
					pline("\"YAY! Lets play together!!\"");
					bindspirit(ep->ward_id);
					u.sealTimeout[IRIS-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					pline("Something grabs your weapon!");
					pline("\"Let me play with that!\"");
					uwep->ovar1_seals |= SEAL_IRIS;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_IRIS;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_IRIS;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[IRIS-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					pline("Apparently it was nothing.");
					// u.sealTimeout[IRIS-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				You("smell sulfur.");
				// u.sealTimeout[IRIS-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case JACK:{
		if(u.sealTimeout[JACK-FIRST_SEAL] < moves){
			//Spirit requires that his seal be drawn outside of hell and the endgame.
			if(!In_hell(&u.uz) && !In_endgame(&u.uz)){
				if(u.sealCounts < numSlots){
					You("feel something climb onto your back!");
					pline("\"Will you let me stay with you?\"");
					bindspirit(ep->ward_id);
					u.sealTimeout[JACK-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					pline("A will-o-wisp drifts out of the seal to hover near your weapon.");
					pline("\"Please let me stay with you!\"");
					uwep->ovar1_seals |= SEAL_JACK;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_JACK;
						u.spiritTineTA= moves + bindingPeriod;
						if(!uwep->lamplit) begin_burn(uwep);
					}
					else{
						u.spiritTineB = SEAL_JACK;
						u.spiritTineTB= moves + bindingPeriod;
						if(!uwep->lamplit) begin_burn(uwep);
					}
					u.sealTimeout[JACK-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					You_hear("aimless footsteps and creaking joints.");
					// u.sealTimeout[JACK-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				You("think of the wide earth.");
				// u.sealTimeout[JACK-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case MALPHAS:{
		if(u.sealTimeout[MALPHAS-FIRST_SEAL] < moves){
			struct obj *otmp, *o=NULL;
			struct monst *mtmp;
			//Spirit requires that his seal be drawn in a square with a fresh corpse.
			for(otmp = level.objects[tx][ty]; otmp; otmp = otmp->nexthere){
				if(otmp->otyp == CORPSE && 
					(otmp->corpsenm == PM_ACID_BLOB
					|| (monstermoves <= peek_at_iced_corpse_age(otmp) + 50)
					) &&
					!(is_rider(&mons[otmp->corpsenm]))
				){
					o = otmp;
			break;
				}
			}
			if(o){
				  /*////////////////////////////////*/
				 /* Do a light sacrificing routine */
				/*////////////////////////////////*/
				if (your_race(&mons[otmp->corpsenm]) 
					&& !is_animal(&mons[otmp->corpsenm]) 
					&& !mindless(&mons[otmp->corpsenm]) 
					&& u.ualign.type != A_VOID
				) {
					/* Human sacrifice on a chaotic or unaligned altar */
					/* is equivalent to demon summoning */
					pline_The("%s blood covers the %s!", urace.adj, surface(tx,ty));
					change_luck(-2);
					
					if (is_demon(youracedata)) {
						You("find the idea of sealing a pact with the blood of your own race to be pleasing.");
						exercise(A_WIS, TRUE);
						adjalign(5);
					} else if (u.ualign.type != A_CHAOTIC) {
						You_feel("you'll regret this infamous offense!");
						(void) adjattrib(A_WIS, -1, TRUE);
						exercise(A_WIS, FALSE);
						adjalign(-5);
						godlist[u.ualign.god].anger += 3;
						if (!Inhell) {
							angrygods(u.ualign.god);
							change_luck(-5);
						}
					}
				} /* your race */
				else if (get_ox(otmp, OX_EMON)
						&& ((mtmp = get_mtraits(otmp, FALSE)) != (struct monst *)0)
						&& mtmp->mtame) {
					/* mtmp is a temporary pointer to a tame monster's attributes,
					 * not a real monster */
					pline("So this is how you repay loyalty?");
					adjalign(-3);
					HAggravate_monster |= TIMEOUT_INF;
				} else if (is_unicorn(&mons[otmp->corpsenm])) {
					int unicalign = sgn((&mons[otmp->corpsenm])->maligntyp);

					if (unicalign == u.ualign.type) {
						pline("This is a grave insult to %s!",
							  (unicalign == A_CHAOTIC)
							  ? "chaos" : unicalign ? "law" : "balance");
						u.ualign.record = -1;
						u.ualign.sins += 10;
					} else{
						if (u.ualign.record < ALIGNLIM)
							You_feel("appropriately %s.", align_str(u.ualign.type));
						else You_feel("you are thoroughly on the right path.");
						adjalign(5);
					}
				}
				Your("sacrifice is accepted.");
				if(!Blind){
					pline("A murder of crows descends on the seal.");
					pline("There is something else in the flock...");
				} else You_hear("many wings.");
				if(u.sealCounts < numSlots){
					if(!Blind) pline("A black-feathered humanoid steps forth.");
					pline("\"I am Malphas. You feed my flock. One way or the other.\"");
					bindspirit(ep->ward_id);
					u.sealTimeout[MALPHAS-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					if(!Blind) pline("A large black feather setles within the seal.");
					pline("\"I am Malphas. With that instrument, you feed my flock.\"");
					uwep->ovar1_seals |= SEAL_MALPHAS;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_MALPHAS;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_MALPHAS;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[MALPHAS-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					if(!Blind) pline("But it does not make itself known to you.");
					// u.sealTimeout[MALPHAS-FIRST_SEAL] = moves + bindingPeriod/10;
				}
				useupf(otmp, 1L);
				newsym(tx,ty);
			} else{
				You("smell fresh blood.");
				// u.sealTimeout[MALPHAS-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case MARIONETTE:{
		if(u.sealTimeout[MARIONETTE-FIRST_SEAL] < moves){
			//Spirit requires that her seal be drawn in the Valley of the Dead or in a graveyard.
			boolean in_a_graveyard = rooms[levl[tx][ty].roomno - ROOMOFFSET].rtype == MORGUE;
			if (in_a_graveyard || on_level(&valley_level, &u.uz) || (tomb_dnum == u.uz.dnum)){
				if(!Blind) You("notice metal wires sticking out of the ground within the seal.");
				if(u.sealCounts < numSlots){
					if(!Blind) pline("In fact, there are wires sticking up all around you.");
					if(!Blind) pline("Shrieks and screams echo down from whence the wires come.");
					else You_hear("screaming!");
					pline("You feel sharp pains in your elbows and knees!");
					if(!Blind) pline("It seems that you, are but a puppet.");
					bindspirit(ep->ward_id);
					u.sealTimeout[MARIONETTE-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					if(!Blind) pline("The wires wrap around your weapon!");
					else pline("Something tangles around your weapon!");
					uwep->ovar1_seals |= SEAL_MARIONETTE;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_MARIONETTE;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_MARIONETTE;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[MARIONETTE-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					pline("But they don't do anything interesting.");
					// u.sealTimeout[MARIONETTE-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				You_hear("the sounds of digging, and of bones rattling together.");
				// u.sealTimeout[MARIONETTE-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case MOTHER:{
		if(u.sealTimeout[MOTHER-FIRST_SEAL] < moves){
			//Spirit requires that her seal addressed while blind.
			if(Blind || LightBlind){
				Your("Hands itch painfully.");
				if(u.sealCounts < numSlots){
					You("feel eyes open in your hands!");
					pline("But you still can't see...");
					pline("...the eyeballs don't belong to you!");
					bindspirit(ep->ward_id);
					u.sealTimeout[MOTHER-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					pline("You're pretty sure something is staring at your weapon....");
					uwep->ovar1_seals |= SEAL_MOTHER;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_MOTHER;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_MOTHER;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[MOTHER-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					pline("The itching subsides.");
					// u.sealTimeout[MOTHER-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				You("blink.");
				// u.sealTimeout[MOTHER-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case NABERIUS:{
		if(u.sealTimeout[NABERIUS-FIRST_SEAL] < moves){
			//Spirit requires that his seal be drawn by an intelligent and wise person.
			if(ACURR(A_INT) >= 14 &&
				ACURR(A_WIS) >= 14 &&
				(u.udrunken >= u.ulevel || Confusion)
			){ 
				You_hear("a snuffing noise.");
				if(u.sealCounts < numSlots){
					if(!Blind){
						pline("A dog wanders in to the seal, nose to the ground.");
						pline("It wanders back and forth, then looks up at you.");
						pline("It looks up at you with all three heads.");
					}
					pline("\"Hello, I am Naberius, the councilor.\"");
					pline("\"I can smell the weaknesses others try to hide.\"");
					pline("\"I can make men trust or make men flee.\"");
					bindspirit(ep->ward_id);
					u.sealTimeout[NABERIUS-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					if(!Blind){
						pline("A dog wanders in to the seal, nose to the ground.");
						pline("With its second head, it licks your hand.");
						pline("But its third head steals your weapon!");
					} else pline("Something steals your weapon!");
					pline("You begin to chase the animal,");
					pline("and it abandons your blade in the center of the seal.");
					uwep->ovar1_seals |= SEAL_NABERIUS;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_NABERIUS;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_NABERIUS;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[NABERIUS-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					pline("The sound fades away.");
					// u.sealTimeout[NABERIUS-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				You_hear("rhetoric and sage advice.");
				// u.sealTimeout[NABERIUS-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case ORTHOS:{
		if(u.sealTimeout[ORTHOS-FIRST_SEAL] < moves){
			struct trap *t=t_at(tx+(tx-u.ux), ty+(ty-u.uy));
			//Spirit requires that his seal be drawn in a square with a hole.
			if(t && t->ttyp == HOLE){
				if(!Blind) pline("The hole grows darker, and a whistling occurs at the edge of hearing.");
				else pline("A whistling occurs at the edge of hearing.");
				pline("The mournful whistle grows louder, as the air around you flows into the pit.");
				if(!Blind) pline("But that is all that occurs. Darkness. Wind. And a lonely whistle.");
				if(u.sealCounts < numSlots){
					pline("You feel that it will stay with you for a while.");
					bindspirit(ep->ward_id);
					u.sealTimeout[ORTHOS-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					Your("blade vibrates for a moment.");
					uwep->ovar1_seals |= SEAL_ORTHOS;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_ORTHOS;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_ORTHOS;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[ORTHOS-FIRST_SEAL] = moves + bindingPeriod;
				} // else u.sealTimeout[ORTHOS-FIRST_SEAL] = moves + bindingPeriod/10;
			} else{
				pline("For an instant you are falling.");
				// u.sealTimeout[ORTHOS-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case OSE:{
		if(u.sealTimeout[OSE-FIRST_SEAL] < moves){
			//Spirit requires that its seal be drawn underwater.
			if(IS_POOL(levl[tx][ty].typ) && IS_POOL(levl[u.ux][u.uy].typ) && u.uinwater && Underwater){ 
				if(u.sealCounts < numSlots){
					pline("The sea-bottom within the seal fades, as if it were silt settling out of muddy water.");
					pline("A sleeping %s floats gently up out of the dark seas below the seal.",u.osegen);
					pline("You suppose %s could be called comely,",u.osepro);
					pline("though to be honest %s is about average among %s you have known.",u.osepro,makeplural(u.osegen));
					if(!rn2(20)) pline("The %s's eyes open, and you have a long negotiation before achieving a good pact.", u.osegen);
					else pline("You know that this is Ose, despite never having met.");
					pline("The seabed rises.");
					bindspirit(ep->ward_id);
					levl[tx][ty].typ = ROOM;
					newsym(tx,ty);
					levl[u.ux][u.uy].typ = ROOM;
					newsym(u.ux,u.uy);
					vision_recalc(2);	/* unsee old position */
					vision_full_recalc = 1;
					spoteffects(FALSE);
					u.sealTimeout[OSE-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					if(!Blind) pline("The sea bottom swirls below your weapon, forming into thousands of reaching arms.");
					uwep->ovar1_seals |= SEAL_OSE;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_OSE;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_OSE;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[OSE-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					if(!Blind) pline("The silt in the seal swirls a bit. Otherwise, nothing happens.");
					// u.sealTimeout[OSE-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else if(In_depths(&u.uz)){ 
				if(u.sealCounts < numSlots){
					pline("There is %s sleeping in the center of the seal.",an(u.osegen));
					pline("You suppose %s could be called comely,",u.osepro);
					pline("though to be honest %s is about average among %s you have known.",u.osepro,makeplural(u.osegen));
					if(!rn2(20)) pline("The %s's eyes open, and you have a long negotiation before achieving a good pact.", u.osegen);
					else pline("You know that this is Ose, despite never having met.");
					bindspirit(ep->ward_id);
					u.sealTimeout[OSE-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					if(!Blind) pline("The dust swirls below your weapon, forming into thousands of reaching arms.");
					uwep->ovar1_seals |= SEAL_OSE;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_OSE;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_OSE;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[OSE-FIRST_SEAL] = moves + bindingPeriod;
				}
				else {
					if(!Blind) pline("The dust in the seal swirls a bit. Otherwise, nothing happens.");
					// u.sealTimeout[OSE-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				You("feel wet....");
				// u.sealTimeout[OSE-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case OTIAX:{
		if(u.sealTimeout[OTIAX-FIRST_SEAL] < moves){
			//Spirit requires that its seal be drawn on a closed door.
			if(IS_DOOR(levl[tx][ty].typ) && closed_door(tx,ty)){ 
				if(!Blind) pline("Thick fingers of mist reach under the door.");
				if(u.sealCounts < numSlots){
					pline("BANG!! Something hits the door from the other side!");
					if(!Blind) pline("The door opens, a bank of fog pours out to curl around you.");
					levl[tx][ty].doormask &= ~(D_CLOSED|D_LOCKED);
					levl[tx][ty].doormask |= D_ISOPEN;
					newsym(tx,ty);
					bindspirit(ep->ward_id);
					u.sealTimeout[OTIAX-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					if(!Blind) pline("The mist fingers curl around your blade.");
					uwep->ovar1_seals |= SEAL_OTIAX;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_OTIAX;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_OTIAX;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[OTIAX-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					if(!Blind) pline("They fade away without incident.");
					// u.sealTimeout[OTIAX-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				You("instinctively look around for a door to open.");
				// u.sealTimeout[OTIAX-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case PAIMON:{
		if(u.sealTimeout[PAIMON-FIRST_SEAL] < moves){
			struct obj *otmp, *o=NULL;
			struct monst *mtmp;
			for(otmp = level.objects[tx][ty]; otmp; otmp = otmp->nexthere){
				if(otmp->oclass == SPBOOK_CLASS && 
					!(
					  otmp->otyp == SPE_BLANK_PAPER ||
					  otmp->otyp == SPE_BOOK_OF_THE_DEAD ||
					  otmp->otyp == SPE_SECRETS
					) && !(otmp->oartifact)
				){
					o = otmp;
			break;
				}
			}
			//Spirit requires that her seal be drawn around a spellbook. The summoner must face toward the northwest during the ritual.
			if( o && tx - u.ux < 0 && ty - u.uy < 0){
				pline("The pages of %s begin to turn.", xname(o));
				if(u.sealCounts < numSlots){
					if(!Blind){
						pline("A beautiful woman rides into the seal on a camel.");
						pline("She scoops up the book and begins pouring through it.");
						pline("As she reads, she absentmindedly hands you her crown.");
					}
					pline("\"Your contribution is appreciated. Now don't bother me.\"");
					o->otyp = SPE_BLANK_PAPER;
					o->obj_color = objects[SPE_BLANK_PAPER].oc_color;
					remove_oprop(o, OPROP_TACTB);
					newsym(tx,ty);
					bindspirit(ep->ward_id);
					u.sealTimeout[PAIMON-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					if(!Blind) pline("A beautiful woman walks into the seal.");
					pline("\"Sometimes, a subtle approach is better.\"");
					if(!Blind) pline("She dips her fingers into the ink of %s and writes on your weapon.", xname(o));
					o->spestudied++;
					uwep->ovar1_seals |= SEAL_PAIMON;
					if(!u.spiritTineA){
						u.spiritTineA = SEAL_PAIMON;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_PAIMON;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[PAIMON-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					if(!Blind) pline("A beautiful woman rides into the seal on a camel.");
					if(!Blind) pline("Her face darkens.");
					if(!Blind) pline("Woman and camel both vanish, replaced by a demon of black smoke.");
					pline("A very masculine voice booms out:");
					pline("\"How dare you waste the time of I, the fell archivist!\".");
					losexp("shredding of the soul",TRUE,TRUE,TRUE);
				}
			} else{
				You("dream briefly of a library in the northwest kingdom.");
				// u.sealTimeout[PAIMON-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case SIMURGH:{
		if(u.sealTimeout[SIMURGH-FIRST_SEAL] < moves){
			//Spirit requires that her seal be drawn outside.
			if(In_outdoors(&u.uz)){
				pline("A brilliantly colored bird with iron claws flies high overhead.");
				if(u.sealCounts < numSlots){
					pline("It swoops down and lands on your shoulder.");
					pline("Its radiant rainbow feathers reflect in its eyes,");
					pline("becoming images of roaring flames and sparkling snow,");
					pline("harsh radiance on barren cliffs");
					pline("and swollen suns in empty black skies.");
					bindspirit(ep->ward_id);
					u.sealTimeout[SIMURGH-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					pline("Loose feathers rain down around your blade,");
					pline("each feather a memory of ruin and of loss.");
					uwep->ovar1_seals |= SEAL_SIMURGH;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_SIMURGH;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_SIMURGH;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[SIMURGH-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					pline("It leaves.");
					// u.sealTimeout[SIMURGH-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else{
				You("yearn for open skies.");
				// u.sealTimeout[SIMURGH-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case TENEBROUS:{
		if(u.sealTimeout[TENEBROUS-FIRST_SEAL] < moves){
			//Spirit requires that his seal be drawn in darkness.
			if( !(levl[tx][ty].lit) &&
					!(viz_array[ty][tx]&TEMP_LIT1 && !(viz_array[ty][tx]&TEMP_DRK3)) && 
				!(levl[u.ux][u.uy].lit) && 
					!(viz_array[u.uy][u.ux]&TEMP_LIT1 && !(viz_array[u.uy][u.ux]&TEMP_DRK3))
			){
				if(!Blind) pline("Within the seal, darkness takes on its own meaning,");
				if(!Blind) pline("beyond mere absence of light.");
				if(u.sealCounts < numSlots){
					if(!Blind) pline("The darkness inside the seal flows out to pool around you.");
					pline("\"None shall rest until my vengeance is complete.");
					pline("All who stand in my way shall face the wrath of that");
					pline("which was wrought in the ancient halls");
					pline("of the realm now known only for dust.\"");
					bindspirit(ep->ward_id);
					u.sealTimeout[TENEBROUS-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					if(!Blind) pline("The darkness inside the seal flows out to stain your weapon.");
					pline("\"None shall rest until my vengeance is complete.\"");
					uwep->ovar1_seals |= SEAL_TENEBROUS;
					if(!u.spiritTineA){ 
						u.spiritTineA = SEAL_TENEBROUS;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_TENEBROUS;
						u.spiritTineTB= moves + bindingPeriod;
					}
					u.sealTimeout[TENEBROUS-FIRST_SEAL] = moves + bindingPeriod;
				}
				else{
					if(!Blind)  pline("Gradually, the lighting returns to normal.");
					// u.sealTimeout[TENEBROUS-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			} else {
				You("think briefly of the dying of the light.");
				// u.sealTimeout[TENEBROUS-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case YMIR:{
		if(u.sealTimeout[YMIR-FIRST_SEAL] < moves){
			struct obj *otmp, *o=NULL;
			struct monst *mtmp;
			for(otmp = level.objects[tx][ty]; otmp; otmp = otmp->nexthere){
				if(	otmp->otyp == CORPSE && 
					otmp->corpsenm != PM_LICHEN && 
					otmp->corpsenm != PM_LIZARD && 
					otmp->corpsenm != PM_BEHOLDER && 
					(monstermoves - peek_at_iced_corpse_age(otmp))/(10L) > 5L &&
					poisonous(&mons[otmp->corpsenm])
				){
					o = otmp;
			break;
				}
			}
			//Spirit requires that his seal be drawn around a rotting corpse of a poisonous creature.
			if(	o ){
				if(!Blind){
					pline("An eye opens on the ground within the seal,");
					pline("and a voice speaks to you out of the Earth:");
				} else pline("A voice speaks to you out of the Earth:");
				pline("\"There was, in times of old, when Ymir lived,\"");
				pline("neither sea nor sand nor waves,");
				pline("no earth, nor heaven above,");
				pline("but a yawning gap, and grass nowhere.");
				pline("From Ymir's flesh the earth was formed,");
				pline("and from his bones the hills.");
				pline("From Ymir's skull, the ice-cold sky,");
				pline("and from his blood the sea.\"");
				if(u.sealCounts < numSlots){
					pline("\"I was Ymir, god of poison,");
					pline("and you are the maggots in my corpse.");
					pline("But I will make a pact with you,");
					pline("to throw down the false gods,");
					pline("that ordered my demise.\"");
					bindspirit(ep->ward_id);
					u.sealTimeout[YMIR-FIRST_SEAL] = moves + bindingPeriod;
				}
				else if(uwep && uwep->oartifact == ART_PEN_OF_THE_VOID && (!u.spiritTineA || (!u.spiritTineB && quest_status.killed_nemesis && Role_if(PM_EXILE)))){
					pline("\"I was Ymir, god of poison,");
					pline("this steel is the steel of my teeth,");
					pline("and the gods shall feel their bite.\"");
					uwep->ovar1_seals |= SEAL_YMIR;
					if(!u.spiritTineA){
						u.spiritTineA = SEAL_YMIR;
						u.spiritTineTA= moves + bindingPeriod;
					}
					else{
						u.spiritTineB = SEAL_YMIR;
						u.spiritTineTB= moves + bindingPeriod;
					}
					if (uwep->opoisoned & OPOISON_BASIC)
						uwep->opoisoned &= ~OPOISON_BASIC;
					u.sealTimeout[YMIR-FIRST_SEAL] = moves + bindingPeriod;
				} else {
					if(!Blind) pline("The eye closes.");
					else pline("But nothing else occurs.");
					// u.sealTimeout[YMIR-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			}  else{
				pline("Rot calls rot, and poison calls poison.");
				// u.sealTimeout[YMIR-FIRST_SEAL] = moves + bindingPeriod/10;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case DAHLVER_NAR:{
		if(u.sealTimeout[DAHLVER_NAR-FIRST_SEAL] < moves){
			//Spirit requires that his seal be drawn by a level 14+ Binder.
			if(quest_status.got_quest && Role_if(PM_EXILE)){
				pline("The bloody, tooth-torn corpse of Dahlver-Nar hangs over the seal.");
				pline("He moans and reaches out to you.");
				bindspirit(ep->ward_id);
				u.sealTimeout[DAHLVER_NAR-FIRST_SEAL] = moves + bindingPeriod;
			}
		} else pline("You hear distant moaning.");
	}break;
	case ACERERAK:{
		if(u.sealTimeout[ACERERAK-FIRST_SEAL] < moves){
			//Spirit requires that his seal be drawn by a Binder who has killed him.
			if(Role_if(PM_EXILE) && quest_status.killed_nemesis){
				pline("A golden skull hangs over the seal.");
				pline("\"I am Acererak. Long ago, I dared the Gates of Teeth.\"");
				pline("\"Now I am trapped outside of time,");
				pline("beyond life, motion, and thought.\"");
				bindspirit(ep->ward_id);
				u.sealTimeout[ACERERAK-FIRST_SEAL] = moves + bindingPeriod;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case COSMOS:{
		struct obj *otmp;
		if(u.sealTimeout[COSMOS-FIRST_SEAL] < moves){
			if(Role_if(PM_EXILE)){
				for(otmp = level.objects[tx][ty]; otmp; otmp = otmp->nexthere) {
					if(otmp->otyp == CRYSTAL_BALL){
						break; // break out of loop
					}
				}
				if(otmp){
					if(!Blind){
						pline("In the heart of the crystal is a tiny, shining light.");
						pline("The light expands, taking the shape of a golden-haired woman.");
					}
					pline("\"I am Cosmos, goddess of memory.\"");
					pline("\"What once was, shall ever be remembered,\"");
					pline("\"preserved in imperishable crystal.\"");
					bindspirit(ep->ward_id);
					u.sealTimeout[COSMOS-FIRST_SEAL] = moves + bindingPeriod;
				} else {
					You("think of shining crystal.");
					// u.sealTimeout[COSMOS-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			}
		} else pline("You can't feel the spirit.");
	}break;
	case LIVING_CRYSTAL:{
		struct obj *otmp;
		if(u.sealTimeout[LIVING_CRYSTAL-FIRST_SEAL] < moves){
			if(Role_if(PM_EXILE)){
				for(otmp = level.objects[tx][ty]; otmp; otmp = otmp->nexthere) {
					if(otmp->oclass == GEM_CLASS && otmp->obj_material == GLASS){
						break; // break out of loop
					}
				}
				if(otmp){
					if(!Blind){
						pline("The motes of dust in the air before you begin to glow.");
						pline("You see a vast, distant gate of crystal through the curtains of bright dustlight.");
						pline("The terrain around the foot of the gate shifts and flows like potters clay,");
						pline("and you hear the murmur of a distant conversation.");
						pline("Suddenly, a patch of darkness and silence appears in the shifting lands,");
						pline("and quickly overtakes all else around the gate.");
						pline("The gate fades, cracks, and shatters.");
						if(!rn2(20))
							You("glimpse a pair of tiny green lights in the place from whence the darkness spread.");
					} else {
						You_hear("the murmur of a distance conversation, and a terrible spreading silence.");
					}
					bindspirit(ep->ward_id);
					u.sealTimeout[LIVING_CRYSTAL-FIRST_SEAL] = moves + bindingPeriod;
				} else {
					You("think of colored glass.");
					// u.sealTimeout[LIVING_CRYSTAL-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			}
		} else pline("You can't feel the spirit.");
	}break;
	case TWO_TREES:{
		struct obj *otmp;
		if(u.sealTimeout[TWO_TREES-FIRST_SEAL] < moves){
			//Spirit requires that her seal be drawn beside a tree or plant.
			if(Role_if(PM_EXILE)){
				if(isok(tx+u.dx, ty+u.dy) && (
					IS_TREE(levl[tx+u.dx][ty+u.dy].typ) 
					|| (m_at(tx+u.dx, ty+u.dy) && m_at(tx+u.dx, ty+u.dy)->data->mlet == S_PLANT)
				)){
					if(!Blind){
						You("see two enormous trees in the distance, growing together atop a quiet hill.");
						You("have an overpowering feeling that you have seen something that no longer exists in this world.");
					} else {
						You("feel like you're trespassing....");
					}
					bindspirit(ep->ward_id);
					u.sealTimeout[TWO_TREES-FIRST_SEAL] = moves + bindingPeriod;
				} else {
					You("think of plants.");
					// u.sealTimeout[TWO_TREES-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			}
		} else pline("You can't feel the spirit.");
	}break;
	case MISKA:{
		if(u.sealTimeout[MISKA-FIRST_SEAL] < moves){
			if(Role_if(PM_EXILE)){
				if(isok(tx+(tx-u.ux), ty+(ty-u.uy)) && 
					is_lava(tx+(tx-u.ux), ty+(ty-u.uy))
				){
					if(!Blind){
						pline("A plume of white smoke rises from the lava.");
						pline("In the shadow of the white plume you see an ancient battlefield.");
						pline("Members of every race and species you have ever seen fight in the battle,");
						pline("some on one side, some on the other, along with many more races and species you have never seen.");
						pline("In the heart of the conflict, a onyx skinned man drives a metallic spear into the heart of a demon of wolves and spiders.");
						pline("There is a flash, and the plume of white smoke is blown apart.");
					} else {
						pline("There is a percussion and a gust of wind.");
					}
					bindspirit(ep->ward_id);
					u.sealTimeout[MISKA-FIRST_SEAL] = moves + bindingPeriod;
				} else {
					You("think of burning lava.");
					// u.sealTimeout[MISKA-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			}
		} else pline("You can't feel the spirit.");
	}break;
	case NUDZIRATH:{
		struct obj *otmp;
		if(u.sealTimeout[NUDZIRATH-FIRST_SEAL] < moves){
			if(Role_if(PM_EXILE) || (u.specialSealsKnown&SEAL_NUDZIRATH)){
				for(otmp = level.objects[tx][ty]; otmp; otmp = otmp->nexthere) {
					if(!otmp->oartifact){
						if(otmp->otyp == MIRROR){
							break; // break out of loop
						}
					}
				}
				if(otmp){
					if(!Blind){
					pline("The mirror in the center of the seal blows apart into a cloud of silver fragments.");
					pline("Reflected in them you see images of long ago.");
					pline("You see a spire of stone; Axis Mundi, that links the boundless sky to the fathomless earth,");
					pline("and a people living at the base of the spire, desiring only solitude and self-contemplation.");
					pline("You see one of their number, different from the rest, build a great library of mirrors,");
					pline("in which all the knowledge of the outside world is reflected.");
					pline("You see her library abandoned, un-entered.");
					pline("But the mirrored library points the way. The people retreat into the realm that lies behind reflections,");
					pline("and there build a mirrored champion, the reflection of a god");
					pline("found sleeping in solitude within a closed-off dimension.");
					pline("And you see shattered dreams.");
					} else pline("You hear a mirror shatter.");
					bindspirit(ep->ward_id);
					u.sealTimeout[NUDZIRATH-FIRST_SEAL] = moves + bindingPeriod;
					useupf(otmp, 1L);
					newsym(tx,ty);
				} else {
					You("are nearly deafened by the sound of mirrors breaking!");
					// u.sealTimeout[NUDZIRATH-FIRST_SEAL] = moves + bindingPeriod/10;
				}
			}
		} else pline("You can't feel the spirit.");
	}break;
	case ALIGNMENT_THING:{
		if(u.sealTimeout[ALIGNMENT_THING-FIRST_SEAL] < moves){
			if(Role_if(PM_EXILE)){
				int a,b;
				You_hear("A babble of voices, asking questions from far, far away.");
				You("look around, trying to find the source of the voices,");
				pline("but you find nothing, until you look Away.");
				You("see there a far-distant realm:");
				pline("planar layers stacked like onion-flesh,");
				pline("twisting possibly-plants growing like ivy on the walls of the universe;");
				pline("presided over by grim, meditating monks whose heads are black thorns.");
				pline("The babble gets louder.");
				pline("You find yourself surrounded by a whirling mass of confused imagery and inane questions:");
				do{
					a = rn2(SIZE(alignmentThings));
					b = rn2(SIZE(alignmentThings));
				}while(a == b);
				pline("%s",alignmentThings[a]);
				pline("%s",alignmentThings[b]);
				pline("You shake free of your trance, but you feel that something sticks to you still....");
				bindspirit(ep->ward_id);
				u.sealTimeout[ALIGNMENT_THING-FIRST_SEAL] = moves + bindingPeriod;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case UNKNOWN_GOD:{
		if(u.sealTimeout[UNKNOWN_GOD-FIRST_SEAL] < moves){
			if(Role_if(PM_EXILE)){
				pline("As you contemplate the seal, your mind fills with unprecedented emotions and indecipherable thoughts,");
				pline("while the world warps and quakes around you.");
				pline("The experience ends as quickly as it began.");
				You("don't know what that was about, but it seems your contract was accepted.");
				make_confused(5, FALSE);
				make_stunned(5, FALSE);
				do_earthquake(u.ux, u.uy, 10, 2, FALSE, (struct monst *)0);
				bindspirit(ep->ward_id);
				u.sealTimeout[UNKNOWN_GOD-FIRST_SEAL] = moves + bindingPeriod;
			}
		} else pline("You can't feel the spirit.");
	}break;
	case BLACK_WEB:{
		if(u.sealTimeout[BLACK_WEB-FIRST_SEAL] < moves){
			struct trap *t = t_at(tx,ty);
			if(t && t->ttyp == WEB && (
				(levl[tx][ty].lit && !(viz_array[ty][tx]&TEMP_DRK3 && !(viz_array[ty][tx]&TEMP_LIT1))) || 
				(viz_array[ty][tx]&TEMP_LIT1 && !(viz_array[ty][tx]&TEMP_DRK3)) || 
				(levl[u.ux][u.uy].lit && !(viz_array[u.uy][u.ux]&TEMP_DRK3 && !(viz_array[u.uy][u.ux]&TEMP_LIT1))) || 
				(viz_array[u.uy][u.ux]&TEMP_LIT1 && !(viz_array[u.uy][u.ux]&TEMP_DRK3))
				)
			){
				pline("The Black Web reaches out from the gate hidden below the white one.");
				pline("Your eyes fill with darkness,");
				pline("your mind with lies,");
				pline("and your veins with poison.");
				pline("The Web stretches on and on below the seal,");
				pline("and the world trembles and shimmers as it falls in.");
				make_hallucinated(5, FALSE, 0L);
				make_blinded(5, FALSE);
				if(!Poison_resistance){
					int typ = rn2(A_MAX);
					
					if (!Fixed_abil) {
						poisontell(typ);
						(void) adjattrib(typ, -rn1(4,3), TRUE);
					}
					exercise(A_CON, FALSE);
				}
				losehp(d(rnd(8),rnd(5)+1), "contact with the Black Web Entity", KILLED_BY);
				bindspirit(ep->ward_id);
				u.sealTimeout[BLACK_WEB-FIRST_SEAL] = moves + bindingPeriod;
			} else You("try to think of the last place you saw a black web....");
		} else pline("You can't feel the spirit.");
	}break;
	case YOG_SOTHOTH:{
		if(u.sealTimeout[YOG_SOTHOTH-FIRST_SEAL] < moves){
			struct trap *t = t_at(tx,ty);
			if((t && (t->ttyp == MAGIC_PORTAL || t->ttyp == LEVEL_TELEP || t->ttyp == TELEP_TRAP)) 
			|| carrying_art(ART_SILVER_KEY)
			){
				You("percieve a great BEING beyond the gate, and it addresses you with waves of thunderous and burning power.");
				You("are smote and changed by the unendurable violence of its voice!");
				exercise(A_CON, FALSE);
				bindspirit(ep->ward_id);
				u.sealTimeout[YOG_SOTHOTH-FIRST_SEAL] = moves + bindingPeriod;
			} else You("need a gateway....");
		} else pline("You can't feel the spirit.");
	}break;
	case NUMINA:{
		//Spirit requires that its seal be drawn by a level 30 Binder.
		//There is no binding period.
		if(u.ulevel == 30 && Role_if(PM_EXILE)){
			int skill;
			You_hear("a tumultuous babble of voices.");
			pline("So insistent are they that even the uninitiated can hear,");
			pline("albeit only in the form of whispers.");
			bindspirit(ep->ward_id);
		} else pline("You hear whispering all around you.");
	}break;
	}
	return MOVE_STANDARD;
}

int *
spirit_props(floorID)
int floorID;
{
#define MAXSPIRITPROPS 3
	static int propchain[MAXSPIRITPROPS + 1];
	int i = 0;

	/* WARNING: by giving a dedicated bit in extrinsics for spirits,
	 * we cannot have two spirits that can be bound simultaneously 
	 * granting duplicate properties
	 */

	switch (floorID)
	{
	case AHAZU:
		break;
	case AMON:
		propchain[i++] = EXTRAMISSION;
		propchain[i++] = COLD_RES;
		break;
	case ANDREALPHUS:
		propchain[i++] = WARN_OF_MON;
		propchain[i++] = TELEPORT_CONTROL;
		break;
	case ANDROMALIUS:
		break;
	case ASTAROTH:
		propchain[i++] = MAGICAL_BREATHING;
		propchain[i++] = SHOCK_RES;
		break;
	case BALAM:
		propchain[i++] = HALF_SPDAM;
		break;
	case BERITH:
		break;
	case BUER:
		break;
	case CHUPOCLOPS:
		propchain[i++] = SLEEP_RES;
		break;
	case DANTALION:
		propchain[i++] = TELEPAT;
		break;
	case ECHIDNA:
		propchain[i++] = ACID_RES;
		break;
	case EDEN:
		propchain[i++] = REFLECTING;
		break;
	case ENKI:
		propchain[i++] = SWIMMING;
		propchain[i++] = WATERPROOF;
		break;
	case EURYNOME:
		propchain[i++] = FREE_ACTION;
		propchain[i++] = WWALKING;
		break;
	case EVE:
		propchain[i++] = HALF_PHDAM;
		break;
	case FAFNIR:
		propchain[i++] = INFRAVISION;
		propchain[i++] = FIRE_RES;
		break;
	case HUGINN_MUNINN:
		propchain[i++] = WARNING;
		propchain[i++] = HALLUC_RES;
		break;
	case IRIS:
		propchain[i++] = SICK_RES;
		break;
	case JACK:
		propchain[i++] = LIFESAVED;
		break;
	case MALPHAS:
		break;
	case MARIONETTE:
		propchain[i++] = STONE_RES;
		break;
	case MOTHER:
		propchain[i++] = CLAIRVOYANT;
		propchain[i++] = ANTIMAGIC;
		break;
	case NABERIUS:
		propchain[i++] = SEE_INVIS;
		break;
	case ORTHOS:
		propchain[i++] = DISPLACED;
		propchain[i++] = DISINT_RES;
		break;
	case OSE:
		propchain[i++] = JUMPING;
		break;
	case OTIAX:
		propchain[i++] = SEARCHING;
		break;
	case PAIMON:
		propchain[i++] = WARN_OF_MON;
		break;
	case SHIRO:
		propchain[i++] = INVIS;
		propchain[i++] = STEALTH;
		break;
	case SIMURGH:
		break;
	case TENEBROUS:
		propchain[i++] = DRAIN_RES;
		break;
	case YMIR:
		propchain[i++] = POISON_RES;
		break;
		/* quest spirits */
	case DAHLVER_NAR:
		propchain[i++] = FIXED_ABIL;
		break;
	case ACERERAK:
		propchain[i++] = WARN_OF_MON;
		propchain[i++] = WELDPROOF;
		break;
	case COUNCIL:
		break;
	case COSMOS:
		break;
	case LIVING_CRYSTAL:
		break;
	case TWO_TREES:
		break;
	case MISKA:
		break;
	case NUDZIRATH:
		propchain[i++] = SPELLBOOST;
		break;
	case ALIGNMENT_THING:
		propchain[i++] = AGGRAVATE_MONSTER;
		propchain[i++] = POLYMORPH_CONTROL;
		break;
	case UNKNOWN_GOD:
		break;
	case BLACK_WEB:
		break;
	case YOG_SOTHOTH:
		propchain[i++] = BLOODSENSE;
		propchain[i++] = PROT_FROM_SHAPE_CHANGERS;
		break;
	case NUMINA:
		propchain[i++] = BLOCK_CONFUSION;
		propchain[i++] = DETECT_MONSTERS;
		break;
	}
	/* add termintor */
	propchain[i] = NO_PROP;
	if(i > MAXSPIRITPROPS)
		impossible("Overfloaw in spirit propery chain.");
	return propchain;
}

int *
spirit_skills(floorID)
int floorID;
{
#define MAXSPIRITSKILLS 5
	static int skillchain[MAXSPIRITSKILLS+1];
	int i = 0;

	switch (floorID)
	{
	case AHAZU:
		skillchain[i++] = P_FLAIL;
		break;
	case AMON:
		skillchain[i++] = P_CLERIC_SPELL;
		break;
	case ANDREALPHUS:
		skillchain[i++] = P_ESCAPE_SPELL;
		break;
	case ANDROMALIUS:
		skillchain[i++] = P_DAGGER;
		break;
	case ASTAROTH:
		skillchain[i++] = P_CROSSBOW;
		skillchain[i++] = P_SHURIKEN;
		break;
	case BALAM:
		skillchain[i++] = P_WHIP;
		break;
	case BERITH:
		skillchain[i++] = P_SABER;
		skillchain[i++] = P_LANCE;
		skillchain[i++] = P_RIDING;
		skillchain[i++] = P_SHIELD;
		break;
	case BUER:
		skillchain[i++] = P_HEALING_SPELL;
		skillchain[i++] = P_BARE_HANDED_COMBAT;
		break;
	case CHUPOCLOPS:
		skillchain[i++] = P_KNIFE;
		break;
	case DANTALION:
		skillchain[i++] = P_BROAD_SWORD;
		skillchain[i++] = P_TWO_HANDED_SWORD;
		skillchain[i++] = P_SCIMITAR;
		break;
	case SHIRO:
		skillchain[i++] = P_POLEARMS;
		break;
	case ECHIDNA:
		skillchain[i++] = P_UNICORN_HORN;
		break;
	case EDEN:
		skillchain[i++] = P_LONG_SWORD;
		break;
	case ENKI:
		skillchain[i++] = P_SHORT_SWORD;
		skillchain[i++] = P_HAMMER;
		skillchain[i++] = P_SLING;
		skillchain[i++] = P_DART;
		skillchain[i++] = P_BOOMERANG;
		break;
	case EURYNOME:
		skillchain[i++] = P_BARE_HANDED_COMBAT;
		break;
	case EVE:
		skillchain[i++] = P_BOW;
		skillchain[i++] = P_HARVEST;
		break;
	case FAFNIR:
		skillchain[i++] = P_PICK_AXE;
		break;
	case HUGINN_MUNINN:
		skillchain[i++] = P_SPEAR;
		break;
	case IRIS:
		skillchain[i++] = P_MORNING_STAR;
		break;
	case JACK:
		break;
	case MALPHAS:
		skillchain[i++] = P_BEAST_MASTERY;
		break;
	case MARIONETTE:
		skillchain[i++] = P_AXE;	
		skillchain[i++] = P_MATTER_SPELL;
		break;
	case MOTHER:
		skillchain[i++] = P_DIVINATION_SPELL;
		break;
	case NABERIUS:
		skillchain[i++] = P_QUARTERSTAFF;
		skillchain[i++] = P_ATTACK_SPELL;
		break;
	case ORTHOS:
		skillchain[i++] = P_MUSICALIZE;
		break;
	case OSE:
		skillchain[i++] = P_TRIDENT;
		break; 
	case OTIAX:
		break; 
	case PAIMON:
		skillchain[i++] = P_WAND_POWER;
		break; 
	case SIMURGH:
		skillchain[i++] = P_ENCHANTMENT_SPELL;
		break; 
	case TENEBROUS:
		skillchain[i++] = P_MACE;
		break; 
	case YMIR:
		skillchain[i++] = P_CLUB;
		break;
	case DAHLVER_NAR:
	case ACERERAK:
	case COUNCIL: /*Council of Elements == crowning*/
	case COSMOS:
	case LIVING_CRYSTAL:
	case TWO_TREES:
		break; 
	case MISKA:
		skillchain[i++] = P_TWO_WEAPON_COMBAT;
		break; 
	case YOG_SOTHOTH:
		skillchain[i++] = P_FIREARM;
		break;
	case NUDZIRATH:
	case ALIGNMENT_THING:
	case UNKNOWN_GOD:
	case BLACK_WEB:
	case NUMINA:
		break;
	}
	/* add termintor */
	skillchain[i] = P_NONE;
	return skillchain;
}

void
bindspirit(floorID)
int floorID;
{
	int bindingPeriod = 5000;
	long sealID = get_sealID(floorID);
	int i;
	int spirit_type;

	/* special cases: Numina, Council of Elements */
	if (floorID == NUMINA) {
		if (u.ulevel == 30 && Role_if(PM_EXILE)){
			int skill;
			for (skill = 0; skill < P_NUM_SKILLS; skill++) {
				if (OLD_P_SKILL(skill) < P_UNSKILLED) OLD_P_SKILL(skill) = P_UNSKILLED;
			}
			u.spirit[OUTER_SPIRIT] = sealID;
			u.specialSealsActive |= sealID;
			u.specialSealsUsed |= sealID;
			set_spirit_powers(sealID);
			/* give properties based on seal */
			int * spiritprops = spirit_props(floorID);
			for (i = 0; spiritprops[i] != NO_PROP; i++)
				u.uprops[spiritprops[i]].extrinsic |= W_SPIRIT;
		}
		else
			You("can't hear the numina.");
		return;

	}
	else if (floorID == COUNCIL) {
		u.specialSealsActive |= sealID;
		u.specialSealsUsed |= sealID;
		set_spirit_powers(sealID);
		return;
	}
	else if (
		floorID == COSMOS ||
		floorID == LIVING_CRYSTAL ||
		floorID == TWO_TREES ||
		floorID == MISKA ||
		floorID == NUDZIRATH ||
		floorID == ALIGNMENT_THING ||
		floorID == YOG_SOTHOTH ||
		floorID == UNKNOWN_GOD
		) {
		spirit_type = ALIGN_SPIRIT;
	}
	else if (
		floorID == ACERERAK ||
		floorID == DAHLVER_NAR ||
		floorID == BLACK_WEB
		) {
		spirit_type = QUEST_SPIRIT;
	}
	else
	{
		if (u.sealCounts == GATE_SPIRITS) {
			unbind(u.spirit[0], FALSE);
		}
		spirit_type = u.sealCounts;
	}

	/* check timeout period */
	if (u.sealTimeout[floorID - FIRST_SEAL] >= moves) {
		/* too soon */
		You("can't feel the spirit.");
		return;
	}

	/* possibly eject spirits */
	if (spirit_type == ALIGN_SPIRIT && u.spirit[ALIGN_SPIRIT]){
		/* Peacefully eject current alignment spirit */
		u.sealTimeout[decode_sealID(u.spirit[ALIGN_SPIRIT]) - FIRST_SEAL] = moves;
		unbind(u.spirit[ALIGN_SPIRIT], FALSE);
	}
	if (spirit_type == QUEST_SPIRIT && u.spirit[QUEST_SPIRIT]){
		/* Forcefully eject current quest spirit */
		if (u.spirit[QUEST_SPIRIT])
			unbind(u.spirit[QUEST_SPIRIT], TRUE);
	}

	/* set common things */
	if (!(sealID & SEAL_SPECIAL))
	{
		u.sealsActive |= sealID;
		u.sealsUsed |= sealID;
		u.sealCounts++;
	}
	else {
		u.specialSealsActive |= sealID;
		u.specialSealsUsed |= sealID;
	}
	set_spirit_powers(sealID);
	u.spirit[spirit_type] = sealID;
	if (!(floorID == BLACK_WEB && Role_if(PM_ANACHRONONAUT))){
		u.spiritT[spirit_type] = moves + bindingPeriod;
		u.sealTimeout[floorID - FIRST_SEAL] = moves + bindingPeriod;
	}

	/* give properties based on seal */
	int * spiritprops = spirit_props(floorID);
	for (i = 0; spiritprops[i] != NO_PROP; i++)
		u.uprops[spiritprops[i]].extrinsic |= W_SPIRIT;

	/* unrestrict skills based on seal */
	int * spiritskills = spirit_skills(floorID);
	for (i = 0; spiritskills[i] != P_NONE; i++)
		unrestrict_weapon_skill(spiritskills[i]);

	/* add to spell-attribute count */
	/* special-seals cannot be part of the wis_spirits long and must be handled separately */
	if ((sealID & SEAL_SPECIAL) ? floorID == DAHLVER_NAR : (sealID & wis_spirits))
		u.wisSpirits++;
	if ((sealID & SEAL_SPECIAL) ? floorID == ACERERAK : (sealID & int_spirits))
		u.intSpirits++;

	/* special effects not covered by anything above */
	switch (floorID)
	{
	case BUER:
		u.umartial = TRUE;
		break;
	case DANTALION:
		u.uiearepairs = TRUE;
		break;
	case ENKI:
		HSwimming |= INTRINSIC;
		break;
	case ORTHOS:
		make_singing_sword_nameable();
		break;
	case TENEBROUS:
		if (Role_if(PM_EXILE) && u.ufirst_life && u.ufirst_sky && u.ufirst_light && !(u.specialSealsKnown&SEAL_LIVING_CRYSTAL)){
			pline("As knowledge of the Echo flows into your mind, you also realize how the Words you've learned can be used in the drawing of a seal!");
			u.specialSealsKnown |= SEAL_LIVING_CRYSTAL;
		}
		check_illumian_trophy();
		break;
	}

	lift_veil();
	vision_full_recalc = 1; //many spirits change what is visible.
	doredraw();
	return;
}

void
councilspirit(floorID)
	int floorID;
{
	int bindingPeriod = 5000, i;
	long old_seal = u.spirit[CROWN_SPIRIT], new_seal = get_sealID(floorID);
	
	
	/* Peacefully eject current crown spirit */
	unbind(old_seal, FALSE);
	/* it does not go on timeout */
	u.sealTimeout[decode_sealID(old_seal) - FIRST_SEAL] = moves;

	/* set standard bound-spirit things */
	u.sealsActive |= new_seal;
	set_spirit_powers(new_seal);
	u.spirit[CROWN_SPIRIT] = new_seal;
	u.spiritT[CROWN_SPIRIT] = moves + bindingPeriod;
	u.sealTimeout[floorID - FIRST_SEAL] = moves + bindingPeriod;

	/* give properties based on seal */
	int * spiritprops = spirit_props(floorID);
	for (i = 0; spiritprops[i] != NO_PROP; i++)
		u.uprops[spiritprops[i]].extrinsic |= W_SPIRIT;

	/* but don't unrestrict skills -- player should have already bound to it */

	/* add to spell-attribute count */
	if (new_seal & wis_spirits)
		u.wisSpirits++;
	if (new_seal & int_spirits)
		u.intSpirits++;	
	
	vision_full_recalc = 1;	/* visible monsters may have changed */
	doredraw();
}

void
gnosisspirit(floorID)
	int floorID;
{
	int bindingPeriod = 5, i;
	long new_seal = get_sealID(floorID);
	
	if(u.spirit[GPREM_SPIRIT] != 0L) unbind(u.spirit[GPREM_SPIRIT], FALSE);
	
	if(new_seal&int_spirits) u.intSpirits++;
	else if(new_seal&wis_spirits) u.wisSpirits++;
	
	u.sealsActive |= new_seal;
	u.spirit[GPREM_SPIRIT] = new_seal;
	set_spirit_powers(new_seal);
	u.spiritT[GPREM_SPIRIT] = moves + bindingPeriod;
//	u.sealTimeout[floorID - FIRST_SEAL] = moves + bindingPeriod;

	/* give properties based on seal */
	int * spiritprops = spirit_props(floorID);
	for (i = 0; spiritprops[i] != NO_PROP; i++)
		u.uprops[spiritprops[i]].extrinsic |= W_SPIRIT;

	/* but don't unrestrict skills -- player should have already bound to it */
	
	vision_full_recalc = 1;	/* visible monsters may have changed */
	doredraw();
}

void
scatter_seals()
{
	struct engr *oep = engr_at(u.ux,u.uy);
	int i;
	long sealID = 0x1L;
	
	for(i = 0; i < (QUEST_SPIRITS-FIRST_SEAL); i++){
		sealID = 0x1L << i;
		if(u.sealsActive&sealID){
			if(!oep){
				make_engr_at(u.ux, u.uy,
				 "", 0L, DUST);
				oep = engr_at(u.ux,u.uy);
			}
			oep->ward_id = (i+FIRST_SEAL);
			oep->halu_ward = 0;
			oep->ward_type = BURN;
			oep->complete_wards = 1;
			rloc_engr(oep);
			oep = engr_at(u.ux,u.uy);
		}
	}
}

static
const char *
DantalionRace(pmon)
int pmon;
{
	switch(pmon){
		case PM_HUMAN:
		case PM_INCANTIFIER:
		case PM_VAMPIRE:
		case PM_CLOCKWORK_AUTOMATON:
		default:
			return "men";
		break;
		case PM_ELF:
			return "elf-lords";
		break;
		case PM_DROW:
			return "hedrow";
		break;
		case PM_DWARF:
			return "dwarves";
		break;
		case PM_GNOME:
			return "gnomes";
		break;
		case PM_ORC:
			return "orcs";
		break;
	}
}

int
P_MAX_SKILL(p_skill)
int p_skill;
{
	return P_MAX_SKILL_CORE(p_skill, TRUE);
}

#define WIND_SKILL(skl) (skl == P_BARE_HANDED_COMBAT || skl == P_AXE || skl == P_SABER || skl == P_HEALING_SPELL || skl == P_ATTACK_SPELL || skl == P_DAGGER)
#define FIRE_SKILL(skl) (skl == P_LONG_SWORD || skl == P_BOW || skl == P_WAND_POWER || skl == P_MATTER_SPELL || skl == P_DAGGER)
#define WATER_SKILL(skl) (skl == P_HAMMER || skl == P_SPEAR || skl == P_BROAD_SWORD || skl == P_CLERIC_SPELL || skl == P_MUSICALIZE)
#define EARTH_SKILL(skl) (skl == P_BARE_HANDED_COMBAT || skl == P_ATTACK_SPELL || skl == P_HEALING_SPELL || skl == P_CLERIC_SPELL || skl == P_SHURIKEN)
#define CHAOS_SKILL(skl) (skl == P_TWO_HANDED_SWORD || skl == P_ESCAPE_SPELL)

#define INCR_MAXSKILL	maxskill = min(max(P_BASIC, maxskill + 1), p_skill == P_BARE_HANDED_COMBAT ? P_GRAND_MASTER : P_EXPERT)
#define INCR_CURSKILL	OLD_P_SKILL(p_skill) == P_ISRESTRICTED ? curskill+=2 : curskill++;


int
P_MAX_SKILL_CORE(p_skill, inc_penalties)
int p_skill;
boolean inc_penalties;
{
	int maxskill = OLD_P_MAX_SKILL(p_skill);
	if(p_skill == P_BARE_HANDED_COMBAT){
		if((u.sealsActive&SEAL_EURYNOME) && (u.sealsActive&SEAL_BUER)) maxskill = max(P_GRAND_MASTER,maxskill);
		else if((u.sealsActive&SEAL_EURYNOME) || (u.sealsActive&SEAL_BUER)) maxskill = max(P_EXPERT,maxskill);
	} else if(spiritSkill(p_skill)) maxskill = max(P_EXPERT,maxskill);
	else if(u.specialSealsActive&SEAL_NUMINA) maxskill = max(P_SKILLED,maxskill);
	
	//if(roleSkill(p_skill)) maxskill = P_EXPERT;

	if(Air_crystal){
		if(WIND_SKILL(p_skill))
			INCR_MAXSKILL;
	}
	if(Fire_crystal){
		if(FIRE_SKILL(p_skill))
			INCR_MAXSKILL;
	}
	if(Water_crystal){
		if(WATER_SKILL(p_skill))
			INCR_MAXSKILL;
	}
	if(Earth_crystal){
		if(EARTH_SKILL(p_skill))
			INCR_MAXSKILL;
	}
	if(Black_crystal){
		if(CHAOS_SKILL(p_skill))
			INCR_MAXSKILL;
	}

	if(p_skill == P_NIMAN){
		if(uwep && uwep->oartifact == ART_INFINITY_S_MIRRORED_ARC)
			maxskill = min(P_EXPERT, P_SKILL(weapon_type(uwep)));
		else if(uswapwep && uswapwep->oartifact == ART_INFINITY_S_MIRRORED_ARC)
			maxskill = min(P_EXPERT, P_SKILL(weapon_type(uswapwep)));
	}
	
	if(inc_penalties && (u.umadness&MAD_FORMICATION || u.umadness&MAD_SCORPIONS) && !BlockableClearThoughts && maxskill > P_UNSKILLED){
		int delta = (Insanity)/20;
		if(Nightmare && ClearThoughts && delta)
			delta = 1; /* Want Should have SOME effect */
		maxskill = max(maxskill - delta, P_UNSKILLED);
	}
	
	return maxskill;
}

int
P_SKILL(p_skill)
int p_skill;
{
	return P_SKILL_CORE(p_skill, TRUE);
}

int
P_SKILL_CORE(p_skill, inc_penalties)
int p_skill;
boolean inc_penalties;
{
	int curskill = OLD_P_SKILL(p_skill),
		maxskill = P_MAX_SKILL(p_skill);
	
	/* Fine motor control drops to 0 while panicking */
	if(inc_penalties && p_skill == P_WAND_POWER && Panicking){
		return 0;
	}
	
	if(p_skill == P_BARE_HANDED_COMBAT){
		if((u.sealsActive&SEAL_EURYNOME) && (u.sealsActive&SEAL_BUER)){
			curskill += 2;
		} else if((u.sealsActive&SEAL_EURYNOME) || (u.sealsActive&SEAL_BUER)){
			curskill += 1;
		}
	} else if(spiritSkill(p_skill)){
		curskill += 1;
	}
	
	/*if(roleSkill(p_skill)){
		curskill = min(curskill+1, P_UNSKILLED);
	}*/
	
	if(p_skill == P_SHIEN){
		if(OLD_P_SKILL(P_DJEM_SO) >= P_SKILLED) curskill++;
		if(OLD_P_SKILL(P_DJEM_SO) >= P_EXPERT) curskill++;
	}
	
	if(p_skill == P_DJEM_SO){
		if(OLD_P_SKILL(P_SHIEN) >= P_SKILLED) curskill++;
		if(OLD_P_SKILL(P_SHIEN) >= P_EXPERT) curskill++;
	}

	curskill = min(curskill, maxskill);

	if(Air_crystal){
		if(WIND_SKILL(p_skill))
			INCR_CURSKILL;
	}
	if(Fire_crystal){
		if(FIRE_SKILL(p_skill))
			INCR_CURSKILL;
	}
	if(Water_crystal){
		if(WATER_SKILL(p_skill))
			INCR_CURSKILL;
	}
	if(Earth_crystal){
		if(EARTH_SKILL(p_skill))
			INCR_CURSKILL;
	}
	if(Black_crystal){
		if(CHAOS_SKILL(p_skill))
			INCR_CURSKILL;
	}
	if(p_skill == P_NIMAN && curskill < P_BASIC){
		if(uwep && uwep->oartifact == ART_INFINITY_S_MIRRORED_ARC){
			curskill = P_BASIC;
			OLD_P_SKILL(P_NIMAN) = P_BASIC;
		} else if(uswapwep && uswapwep->oartifact == ART_INFINITY_S_MIRRORED_ARC) {
			curskill = P_BASIC;
			OLD_P_SKILL(P_NIMAN) = P_BASIC;
		}
	}
	
	if(u.sealsActive&SEAL_NABERIUS && (curskill<P_BASIC || maxskill<P_BASIC)){
		curskill = P_BASIC;
	}
	
	if(inc_penalties && (u.umadness&MAD_FORMICATION || u.umadness&MAD_SCORPIONS) && !BlockableClearThoughts && curskill > P_UNSKILLED){
		int delta = (Insanity)/20;
		if(Nightmare && ClearThoughts && delta)
			delta = 1; /* Want Should have SOME effect */
		curskill = max(curskill - delta, P_UNSKILLED);
	}

	return curskill;
}

int
P_RESTRICTED(p_skill)
int p_skill;
{
	if(p_skill == P_NIMAN){
		if(uwep && uwep->oartifact == ART_INFINITY_S_MIRRORED_ARC)
			return P_RESTRICTED(weapon_type(uwep));
		else if(uswapwep && uswapwep->oartifact == ART_INFINITY_S_MIRRORED_ARC)
			return P_RESTRICTED(weapon_type(uswapwep));
	}
	return (P_SKILL(p_skill) == P_ISRESTRICTED);
}

/*
 * this has been co-opted into meaning
 * "skill unlocked by specific role under specific circumstances"
 */
boolean
roleSkill(p_skill)
int p_skill;
{
	if (Role_if(PM_KNIGHT)){
		if (p_skill == P_KNI_SACRED)
			return TRUE;
		if (p_skill == P_KNI_ELDRITCH)
			return TRUE;
		if (p_skill == P_KNI_RUNIC)
			return TRUE;
	}
	return FALSE;
}

boolean
spiritSkill(p_skill)
int p_skill;
{
	static long seal_skill_list[P_NUM_SKILLS];
	static long specialseal_skill_list[P_NUM_SKILLS];
	static boolean madelist = FALSE;

	if (!madelist) {
		/* must create list */
		madelist = TRUE;
		int floorID;
		int * list;
		int i;
		for (floorID = FIRST_SEAL; floorID <= NUMINA; floorID++) {
			list = spirit_skills(floorID);
			for (i = 0; list[i] != P_NONE; i++) {
				if (floorID < QUEST_SPIRITS)
					seal_skill_list[list[i]] |= get_sealID(floorID);
				else
					specialseal_skill_list[list[i]] |= (get_sealID(floorID) & ~SEAL_SPECIAL);
			}
		}
	}

	if (u.sealsActive & seal_skill_list[p_skill])
		return TRUE;
	if (u.specialSealsActive & specialseal_skill_list[p_skill])
		return TRUE;

	/* special cases */
	if (u.sealsActive & SEAL_BERITH && u.usteed && p_skill == P_BOW)
		return TRUE;

	return FALSE;
}
#ifdef USER_SOUNDS

extern void FDECL(play_usersound, (const char*, int));

typedef struct audio_mapping_rec {
#ifdef USER_SOUNDS_REGEX
	struct re_pattern_buffer regex;
#else
	char *pattern;
#endif
	char *filename;
	int volume;
	struct audio_mapping_rec *next;
} audio_mapping;

static audio_mapping *soundmap = 0;

char* sounddir = ".";

/* adds a sound file mapping, returns 0 on failure, 1 on success */
int
add_sound_mapping(mapping)
const char *mapping;
{
	char text[256];
	char filename[256];
	char filespec[256];
	int volume;

	if (sscanf(mapping, "MESG \"%255[^\"]\"%*[\t ]\"%255[^\"]\" %d",
		   text, filename, &volume) == 3) {
	    const char *err;
	    audio_mapping *new_map;

	    if (strlen(sounddir) + strlen(filename) > 254) {
		raw_print("sound file name too long");
		return 0;
	    }
	    Sprintf(filespec, "%s/%s", sounddir, filename);

	    if (can_read_file(filespec)) {
		new_map = (audio_mapping *)alloc(sizeof(audio_mapping));
#ifdef USER_SOUNDS_REGEX
		new_map->regex.translate = 0;
		new_map->regex.fastmap = 0;
		new_map->regex.buffer = 0;
		new_map->regex.allocated = 0;
		new_map->regex.regs_allocated = REGS_FIXED;
#else
		new_map->pattern = (char *)alloc(strlen(text) + 1);
		Strcpy(new_map->pattern, text);
#endif
		new_map->filename = strdup(filespec);
		new_map->volume = volume;
		new_map->next = soundmap;

#ifdef USER_SOUNDS_REGEX
		err = re_compile_pattern(text, strlen(text), &new_map->regex);
#else
		err = 0;
#endif
		if (err) {
		    raw_print(err);
		    free(new_map->filename);
		    free(new_map);
		    return 0;
		} else {
		    soundmap = new_map;
		}
	    } else {
		Sprintf(text, "cannot read %.243s", filespec);
		raw_print(text);
		return 0;
	    }
	} else {
	    raw_print("syntax error in SOUND");
	    return 0;
	}

	return 1;
}

void
play_sound_for_message(msg)
const char* msg;
{
	audio_mapping* cursor = soundmap;

	while (cursor) {
#ifdef USER_SOUNDS_REGEX
	    if (re_search(&cursor->regex, msg, strlen(msg), 0, 9999, 0) >= 0) {
#else
	    if (pmatch(cursor->pattern, msg)) {
#endif
		play_usersound(cursor->filename, cursor->volume);
	    }
	    cursor = cursor->next;
	}
}

#endif /* USER_SOUNDS */

STATIC_OVL int
doyochlolmenu(mon)
struct monst *mon;
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	Sprintf(buf, "Change to which form?");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	
	incntlet = 'a';
	
	if(mon->mtyp != PM_YOCHLOL){
		Sprintf(buf, "Yochlol");
		any.a_int = PM_YOCHLOL;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++;
	if(mon->mtyp != PM_UNEARTHLY_DROW){
		Sprintf(buf, "Drow");
		any.a_int = PM_UNEARTHLY_DROW;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++;
	if(mon->mtyp != PM_STINKING_CLOUD){
		Sprintf(buf, "Cloud");
		any.a_int = PM_STINKING_CLOUD;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++;
	if(mon->mtyp != PM_DEMONIC_BLACK_WIDOW){
		Sprintf(buf, "Spider");
		any.a_int = PM_DEMONIC_BLACK_WIDOW;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++;
	
	end_menu(tmpwin, "Select form");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	if(n > 0){
		int picked = selected[0].item.a_int;
		free(selected);
		return picked;
	}
	return 0;
}

STATIC_OVL int
doblessmenu()
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	Sprintf(buf, "Ask for blessing?");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	
	incntlet = 'a';
	
	Sprintf(buf, "Drive out curses");
	any.a_int = BLESS_CURSES;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet++;
	
	Sprintf(buf, "Blessing of good fortune");
	any.a_int = BLESS_LUCK;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet++;
	
	if(uwep){
		Sprintf(buf, "Bless wielded item");
		any.a_int = BLESS_WEP;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++; //Advance anyway
	if(Sterile){
		Sprintf(buf, "Lift sterility curse");
		any.a_int = UNSTERILIZE;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++; //Advance anyway
	if(uwep && ((accepts_weapon_oprops(uwep) && !check_oprop(uwep, OPROP_HOLYW)) ||
		    (uwep->oclass == ARMOR_CLASS && !check_oprop(uwep, OPROP_HOLY)))){
		Sprintf(buf, "Sanctify your weapon");
		any.a_int = SANCTIFY_WEP;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++; //Advance anyway
	
	end_menu(tmpwin, "Select blessing");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	if(n > 0){
		int picked = selected[0].item.a_int;
		free(selected);
		return picked;
	}
	return 0;
}

int
donursemenu()
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	Sprintf(buf, "Ask for treatment?");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	
	incntlet = 'a';
	
	Sprintf(buf, "Fortify my health ($%d)", nurseprices[NURSE_FULL_HEAL]);
	any.a_int = NURSE_FULL_HEAL;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet++;
	if(u.usanity < 100){
		Sprintf(buf, "Something for my nerves ($%d)", nurseprices[NURSE_TRANQUILIZERS]);
		any.a_int = NURSE_TRANQUILIZERS;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++; //Advance anyway
	
	if(ABASE(A_STR) < (AMAX(A_STR) - (u.uhs >= 3 ? 1 : 0))
	|| ABASE(A_DEX) < AMAX(A_DEX)
	|| ABASE(A_CON) < AMAX(A_CON)
	|| ABASE(A_INT) < AMAX(A_INT)
	|| ABASE(A_WIS) < AMAX(A_WIS)
	|| ABASE(A_CHA) < AMAX(A_CHA)
	){
		Sprintf(buf, "Steroids ($%d)", nurseprices[NURSE_RESTORE_ABILITY]); /*Note: to accelerate natural healing*/
		any.a_int = NURSE_RESTORE_ABILITY;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++; //Advance anyway
	if(u.umorgul > 0){
		Sprintf(buf, "Extract morgul shards ($%d)", nurseprices[NURSE_FIX_MORGUL]);
		any.a_int = NURSE_FIX_MORGUL;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++; //Advance anyway
	if(Sick){
		Sprintf(buf, "Antibiotics ($%d)", nurseprices[NURSE_FIX_SICKNESS]);
		any.a_int = NURSE_FIX_SICKNESS;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++; //Advance anyway
	if(Slimed){
		Sprintf(buf, "Remove slimy green growths ($%d)", nurseprices[NURSE_FIX_SLIME]);
		any.a_int = NURSE_FIX_SLIME;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++; //Advance anyway
	if(Sterile){
		Sprintf(buf, "Fertility treatment ($%d)", nurseprices[NURSE_FIX_STERILE]);
		any.a_int = NURSE_FIX_STERILE;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++; //Advance anyway
	if(u.thoughts && !uarmh){
		Sprintf(buf, "Brain surgery ($%d)", nurseprices[NURSE_BRAIN_SURGERY]);
		any.a_int = NURSE_BRAIN_SURGERY;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++; //Advance anyway
	
	end_menu(tmpwin, "Select treatment");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	if(n > 0){
		int picked = selected[0].item.a_int;
		free(selected);
		return picked;
	}
	return 0;
}

boolean
nurse_services(nurse)
struct monst *nurse;
{
	int service, gold, count = 1, cost;
	
	if((uarm || uarmu) && (!u.thoughts || uarmh))
		return FALSE;
	
	service = donursemenu();
	if(!service)
		return FALSE;
	
#ifndef GOLDOBJ
		gold = u.ugold;
#else
		gold = money_cnt(invent);
#endif
	if(service == NURSE_FULL_HEAL || service == NURSE_TRANQUILIZERS){
		char inbuf[BUFSZ];
		getlin("How many courses?", inbuf);
		if (*inbuf == '\033') count = 1;
		else count = atoi(inbuf);
		if(count == 0){
			pline("Never mind.");
			return FALSE;
		}
		if(count < 0)
			count = 1;
	}
	cost = nurseprices[service]*count;
	if(!nurse->mtame)
		cost += cost/10; //10% surcharge
	
	if(gold < cost){
		pline("You don't have enough gold!");
		return FALSE;
	} else {
		pline("That will be $%d.", cost);
		if(yn("Pay?") != 'y')
			return FALSE;
	}
	switch(service){
		case NURSE_FULL_HEAL:
			pline("%s doses you with healing medicine.", Monnam(nurse));
			healup(400*count, 8*count, FALSE, TRUE);
		break;
		case NURSE_TRANQUILIZERS:
			pline("%s doses you with tranquilizers.", Monnam(nurse));
			if(Sleep_resistance || Free_action)
				You("yawn.");
			else {
				You("suddenly fall asleep!");
				fall_asleep(-13*count, TRUE);
			}
			//Sedative
			change_usanity(15*count, FALSE);
		break;
		case NURSE_RESTORE_ABILITY:{
			int i, lim;
			pline("%s doses you with ability restoring medicine.", Monnam(nurse));
		    pline("Wow!  You feel %s!",
			  unfixable_trouble_count(FALSE) ? "better" : "great"
			 );
		    for (i = 0; i < A_MAX; i++) {
				lim = AMAX(i);
				if (i == A_STR && u.uhs >= 3) --lim;	/* WEAK */
				if (ABASE(i) < lim){
					ABASE(i) = lim;
					flags.botl = 1;
				}
		    }
		}break;
		case NURSE_FIX_MORGUL:{
			int i = u.umorgul;
			struct obj *frags;
			u.umorgul = 0;
			frags = mksobj(SHURIKEN, MKOBJ_NOINIT);
			pline("%s performs surgery, removing %d metallic shard%s from your body.", Monnam(nurse), i, (i>1) ? "s" : "");
			if(frags){
				frags->quan = i;
				add_oprop(frags, OPROP_LESSER_MORGW);
				set_material_gm(frags, METAL);
				curse(frags);
				fix_object(frags);
				frags = hold_another_object(frags, "You drop %s!",
							  doname(frags), (const char *)0); /*shouldn't merge, but may drop*/
			}
		}break;
		case NURSE_FIX_SICKNESS:
			pline("%s doses you with antibiotics.", Monnam(nurse));
			healup(0, 0, TRUE, FALSE);
		break;
		case NURSE_FIX_SLIME:
			pline("%s burns away the slimy growths.", Monnam(nurse));
			Slimed = 0L;
		break;
		case NURSE_FIX_STERILE:
			pline("%s doses you with fertility medicine.", Monnam(nurse));
			HSterile = 0L;
		break;
		case NURSE_BRAIN_SURGERY:{
			int otyp;
			struct obj *glyph;
			otyp = dotrephination_menu();
			if(!otyp)
				break;
			
			glyph = mksobj(otyp, MKOBJ_NOINIT);
			
			if(glyph){
				remove_thought(otyp_to_thought(otyp));
				if(Race_if(PM_ANDROID)){
					set_material_gm(glyph, PLASTIC);
					fix_object(glyph);
				}
				if(Race_if(PM_CLOCKWORK_AUTOMATON)){
					set_material_gm(glyph, COPPER);
					fix_object(glyph);
				}
				if(Race_if(PM_WORM_THAT_WALKS)){
					set_material_gm(glyph, SHELL_MAT);
					fix_object(glyph);
				}
				hold_another_object(glyph, "You drop %s!", doname(glyph), (const char *)0);
				if(ACURR(A_WIS)>ATTRMIN(A_WIS)){
					adjattrib(A_WIS, -1, FALSE);
				}
				if(ACURR(A_INT)>ATTRMIN(A_INT)){
					adjattrib(A_INT, -1, FALSE);
				}
				if(ACURR(A_CON)>ATTRMIN(A_CON)){
					adjattrib(A_CON, -1, FALSE);
				}
				change_usanity(-10, TRUE);
				//Note: this is always the player's HP, not their polyform HP.
				u.uhp -= u.uhp/2; //Note: chopped, so 0 to 1/2 max-HP lost.
			} else {
				impossible("Shard creation failed during nurse brain surgery??");
			}
		}break;
	}
#ifndef GOLDOBJ
	u.ugold -= cost;
	if(!nurse->mtame)
		nurse->mgold += nurseprices[service]*count/10;
#else
	money2none(nurseprices[service]*count);
	if(!nurse->mtame)
		(void) money2mon(nurse, nurseprices[service]*count/10);
#endif
	return TRUE;
}

int
dorendermenu()
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	Sprintf(buf, "Ask for aid?");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	
	incntlet = 'a';

	if(u.umorgul > 0){
		Sprintf(buf, "Extract morgul shards");
		any.a_int = RENDER_FIX_MORGUL;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++; //Advance anyway
	if(Sick){
		Sprintf(buf, "Extract pathogen");
		any.a_int = RENDER_FIX_SICKNESS;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++; //Advance anyway
	if(Slimed){
		Sprintf(buf, "Remove slimy green growths");
		any.a_int = RENDER_FIX_SLIME;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++; //Advance anyway
	if(u.thoughts){
		Sprintf(buf, "Extract thought");
		any.a_int = RENDER_BRAIN_SURGERY;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++; //Advance anyway
	//Pick out parasitic eggs
	
	if(count_glyphs() < MAX_GLYPHS && !u.render_thought){
		Sprintf(buf, "Learn thought");
		any.a_int = RENDER_THOUGHT;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++; //Advance anyway
	
	end_menu(tmpwin, "");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	if(n > 0){
		int picked = selected[0].item.a_int;
		free(selected);
		return picked;
	}
	return 0;
}

boolean
render_services(render)
struct monst *render;
{
	int service, gold, count = 1, cost;
		
	service = dorendermenu();
	if(!service)
		return FALSE;
	
	switch(service){
		case RENDER_FIX_MORGUL:{
			int i = u.umorgul;
			struct obj *frags;
			u.umorgul = 0;
			frags = mksobj(SHURIKEN, MKOBJ_NOINIT);
			pline("%s reaches into your body, removing %d metallic shard%s.", Monnam(render), i, (i>1) ? "s" : "");
			change_usanity(-10, TRUE);
			if(frags){
				frags->quan = i;
				add_oprop(frags, OPROP_LESSER_MORGW);
				set_material_gm(frags, METAL);
				curse(frags);
				fix_object(frags);
				frags = hold_another_object(frags, "You drop %s!",
							  doname(frags), (const char *)0); /*shouldn't merge, but may drop*/
			}
		}break;
		case RENDER_FIX_SICKNESS:
			pline("%s reaches into your body, removing some sort of slime!", Monnam(render));
			healup(0, 0, TRUE, FALSE);
		break;
		case RENDER_FIX_SLIME:
			pline("%s picks off the slimy growths.", Monnam(render));
			Slimed = 0L;
		break;
		case RENDER_BRAIN_SURGERY:{
			int otyp;
			struct obj *glyph;
			otyp = dotrephination_menu();
			if(!otyp)
				break;
			
			glyph = mksobj(otyp, MKOBJ_NOINIT);
			
			if(glyph){
				remove_thought(otyp_to_thought(otyp));
				if(Race_if(PM_ANDROID)){
					set_material_gm(glyph, PLASTIC);
					fix_object(glyph);
				}
				if(Race_if(PM_CLOCKWORK_AUTOMATON)){
					set_material_gm(glyph, COPPER);
					fix_object(glyph);
				}
				if(Race_if(PM_WORM_THAT_WALKS)){
					set_material_gm(glyph, SHELL_MAT);
					fix_object(glyph);
				}
				hold_another_object(glyph, "You drop %s!", doname(glyph), (const char *)0);
				if(ACURR(A_WIS)>ATTRMIN(A_WIS)){
					adjattrib(A_WIS, -1, FALSE);
				}
				if(ACURR(A_INT)>ATTRMIN(A_INT)){
					adjattrib(A_INT, -1, FALSE);
				}
				if(ACURR(A_CON)>ATTRMIN(A_CON)){
					adjattrib(A_CON, -1, FALSE);
				}
				change_usanity(-10, TRUE);
				//Note: this is always the player's HP, not their polyform HP.
				u.uhp -= u.uhp/2; //Note: chopped, so 0 to 1/2 max-HP lost.
			} else {
				impossible("Shard creation failed during render brain surgery??");
			}
		}break;
		case RENDER_THOUGHT:
			if(!dofreethought())
				return FALSE;
			else {
				u.render_thought = TRUE;
			}
		break;
	}
	return TRUE;
}

int
dodollmenu(dollmaker)
struct monst *dollmaker;
{
	winid tmpwin;
	int n, how;
	long l;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	Sprintf(buf, "Buy a doll?");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	
	incntlet = 'a';
	
	for(l = 0x1L, n = EFFIGY; l <= MAX_DOLL_MASK; l=(l<<1), n++){
		if(dollmaker->mvar_dollTypes&l){
			if(objects[n].oc_name_known)
				Sprintf(buf, "%s ($%d)", OBJ_NAME(objects[n]), 800);
			else
				Sprintf(buf, "%s ($%d)", OBJ_DESCR(objects[n]), 800);
			any.a_int = n;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				incntlet, 0, ATR_NONE, buf,
				MENU_UNSELECTED);
		}
		incntlet++; //Advance anyway
	}
	
	if(is_dollable(dollmaker->data)){
		Sprintf(buf, "doll tear ($%d)", 8000);
		any.a_int = DOLL_S_TEAR;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	incntlet++; //Advance anyway
	
	end_menu(tmpwin, "Select doll type");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	if(n > 0){
		int picked = selected[0].item.a_int;
		free(selected);
		return picked;
	}
	return 0;
}

boolean
buy_dolls(dollmaker)
struct monst *dollmaker;
{
	int dollnum, gold, count = 1, cost, basecost;
	struct obj *doll;
	
	dollnum = dodollmenu(dollmaker);
	if(!dollnum)
		return FALSE;
	
#ifndef GOLDOBJ
		gold = u.ugold;
#else
		gold = money_cnt(invent);
#endif
	if(dollnum != DOLL_S_TEAR){
		char inbuf[BUFSZ];
		basecost = 800;
		getlin("How many?", inbuf);
		if (*inbuf == '\033') count = 1;
		else count = atoi(inbuf);
		if(count == 0){
			pline("Never mind.");
			return FALSE;
		}
		if(count < 0)
			count = 1;
	} else {
		basecost = 8000;
	}
	cost = basecost*count;
	
	if(gold < cost){
		pline("You don't have enough gold!");
		return FALSE;
	} else {
		pline("That will be $%d.", cost);
		if(yn("Pay?") != 'y')
			return FALSE;
	}
	
	doll = mksobj(dollnum, MKOBJ_NOINIT);
	if(!doll){
		impossible("doll creation failed?");
		return FALSE;
	}
	if(dollnum == DOLL_S_TEAR){
		doll->ovar1_dollTypes = dollmaker->mvar_dollTypes;
		doll->spe = (char)dollmaker->m_insight_level;
		dollmaker->m_insight_level = 0;
		mondied(dollmaker);
	}
	doll->quan = count;
	doll->owt = weight(doll);
	hold_another_object(doll, "You drop %s!",
				doname(doll), (const char *)0);
	
#ifndef GOLDOBJ
	u.ugold -= cost;
#else
	money2none(cost);
#endif
	return TRUE;
}

#endif /* OVLB */

/*sounds.c*/
