/*	SCCS Id: @(#)thoughtglyph.h	3.4	1997/05/01	*/
/* Copyright (c) Izchak Miller, 1989.				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef MUTATION_H
#define MUTATION_H

/* Macros for messages referring to hands, eyes, feet, etc... */
#define ARM 0
#define EYE_BP 1
#define FACE 2
#define FINGER 3
#define FINGERTIP 4
#define FOOT 5
#define HAND 6
#define HANDED 7
#define HEAD 8
#define LEG 9
#define LIGHT_HEADED 10
#define NECK 11
#define SPINE 12
#define TOE 13
#define HAIR 14
#define BLOOD 15
#define LUNG 16
#define NOSE 17
#define STOMACH 18
#define HEART 19
#define BODY_SKIN 20
#define BODY_FLESH 21
#define BEAT 22
#define BONES 23
#define EAR 24
#define EARS 25
#define TONGUE 26
#define BRAIN 27
#define CREAK 28
#define CRACK 29
#define THROAT 30
#define WINDPIPE 31
#define WINGS_BP 32
#define LAST_BP 32

#define SMELL_TRAIT	LAST_BP+1
#define MOUTH_TRAIT	LAST_BP+2
#define TAIL_TRAIT	LAST_BP+3
#define HORN_TRAIT	LAST_BP+4
#define SHADOW_TRAIT	LAST_BP+5
#define SOUND_TRAIT	LAST_BP+6
#define AURA_TRAIT	LAST_BP+7
#define APPEARANCE_TRAIT	LAST_BP+8
#define MUTATION_TYPE_SIZE	APPEARANCE_TRAIT+1

struct mutationtype {
	int mutation;
	int bodypart;
	const char * name;
	const char * description;
	const char * forming_name;
	const char * start_forming;
};

/* mutations (Must be non-zero) */
#define	ABHORRENT_SPORE	1
#define	CRAWLING_FLESH	2
#define	SHUB_RADIANCE	3
#define	TENDRIL_HAIR	4
#define	SHIFTING_MIND	5
#define	SHUB_CLAWS		6
#define	MIND_STEALER	7
#define	YOG_GAZE_1		8
#define	YOG_GAZE_2		9
#define	TWIN_MIND		10
#define	TWIN_DREAMS		11
#define	BY_THE_SMELL		12
#define	TWIN_SAVE		13
#define	SHUB_TENTACLES		14
#define LAST_CULT_MUTATION		SHUB_TENTACLES
//Tiefling Traits
#define	TT_POISON_CLOUD		LAST_CULT_MUTATION+1
#define	TT_FIRE_BLAST_1		LAST_CULT_MUTATION+2
#define	TT_FIRE_BLAST_2		LAST_CULT_MUTATION+3
#define	TT_COLD_BLAST		LAST_CULT_MUTATION+4
#define	TT_ACID_BLAST		LAST_CULT_MUTATION+5
#define	TT_NA_SCALES		LAST_CULT_MUTATION+6
#define	TT_DR_SCALES		LAST_CULT_MUTATION+7
#define	TT_SCALES			LAST_CULT_MUTATION+8
#define	TT_CHITIN			LAST_CULT_MUTATION+9
#define	TT_SLIPPERY_SKIN	LAST_CULT_MUTATION+10
#define	TT_FUNGUS_SKIN		LAST_CULT_MUTATION+11
#define	TT_SNAKE_FANGS		LAST_CULT_MUTATION+12
#define	TT_VAMPIRE_FANGS	LAST_CULT_MUTATION+13
#define TT_SPIDER_FANGS		LAST_CULT_MUTATION+14
#define TT_SERPENT			LAST_CULT_MUTATION+15
#define TT_BLINDING_VENOM	LAST_CULT_MUTATION+16
#define TT_SMOKE			LAST_CULT_MUTATION+17
#define TT_COLD_CLOUD		LAST_CULT_MUTATION+18
#define TT_HATEFUL_VISION	LAST_CULT_MUTATION+19
#define TT_ODD_EYES_1		LAST_CULT_MUTATION+20
#define TT_ODD_EYES_2		LAST_CULT_MUTATION+21
#define TT_ODD_EYES_3		LAST_CULT_MUTATION+22
#define TT_MANY_ODD_EYES	LAST_CULT_MUTATION+23
#define TT_INFRAVISION_1	LAST_CULT_MUTATION+24
#define TT_EXTRAMISSION_1	LAST_CULT_MUTATION+25
#define TT_EXTRAMISSION_2	LAST_CULT_MUTATION+26
#define TT_BEHOLDER			LAST_CULT_MUTATION+27
#define TT_LIGHT			LAST_CULT_MUTATION+28
#define TT_DISCOVERY_1		LAST_CULT_MUTATION+29
#define TT_DISCOVERY_2		LAST_CULT_MUTATION+30
#define TT_PARALYSIS_GAZE	LAST_CULT_MUTATION+31
#define TT_CANCEL_GAZE		LAST_CULT_MUTATION+32
#define TT_PROBING_GAZE		LAST_CULT_MUTATION+33
#define TT_MESMERIZING_GAZE	LAST_CULT_MUTATION+34
#define TT_CLOCKWORK_EYES	LAST_CULT_MUTATION+35
#define TT_TEARS_OF_BLOOD	LAST_CULT_MUTATION+36
#define TT_COLD_TOUCH		LAST_CULT_MUTATION+37
#define TT_DRAIN_TOUCH		LAST_CULT_MUTATION+38
#define TT_FIRE_TOUCH		LAST_CULT_MUTATION+39
#define TT_SHOCK_TOUCH		LAST_CULT_MUTATION+40
#define TT_EXTRA_FINGERS	LAST_CULT_MUTATION+41
#define TT_WEBS				LAST_CULT_MUTATION+42
#define TT_SHADOW_PAIN		LAST_CULT_MUTATION+43
#define TT_SHADOW_SHRED		LAST_CULT_MUTATION+44
#define TT_WANDERING_SHADOW	LAST_CULT_MUTATION+45
#define TT_SHADOW_CASTER	LAST_CULT_MUTATION+46
#define TT_FIRE_COUNTER		LAST_CULT_MUTATION+47
#define TT_COLD_COUNTER		LAST_CULT_MUTATION+48
#define TT_ACID_COUNTER		LAST_CULT_MUTATION+49
#define TT_POISON_COUNTER	LAST_CULT_MUTATION+50
#define TT_FROG_CROAK		LAST_CULT_MUTATION+51
#define TT_ECHOLOCATION		LAST_CULT_MUTATION+52
#define TT_SIREN_SONG		LAST_CULT_MUTATION+53
#define TT_THORN_HAIR		LAST_CULT_MUTATION+54
#define TT_FLAMING_HAIR		LAST_CULT_MUTATION+55
#define TT_FROSTY_HAIR		LAST_CULT_MUTATION+56
#define TT_BLINDING_HAIR	LAST_CULT_MUTATION+57
#define TT_BITING_HAIR		LAST_CULT_MUTATION+58
#define TT_RAMS_HORN		LAST_CULT_MUTATION+59
#define TT_DEMON_HORN		LAST_CULT_MUTATION+60
#define TT_UNICORN_HORN		LAST_CULT_MUTATION+61
#define TT_ANTLERS			LAST_CULT_MUTATION+62
#define TT_BULL_HORNS		LAST_CULT_MUTATION+63
#define TT_RAZOR_CLAWS		LAST_CULT_MUTATION+64
#define TT_HARD_CLAWS		LAST_CULT_MUTATION+65
#define TT_HOOKED_CLAWS		LAST_CULT_MUTATION+66
#define TT_TALONS			LAST_CULT_MUTATION+67
#define TT_PREHENSILE_TAIL	LAST_CULT_MUTATION+68
#define TT_LASHING_TAIL		LAST_CULT_MUTATION+69
#define TT_STINGER_TAIL		LAST_CULT_MUTATION+70
#define TT_SNAKE_TAIL		LAST_CULT_MUTATION+71
#define TT_THIEVING_TAIL	LAST_CULT_MUTATION+72
#define TT_LIZARD_TAIL		LAST_CULT_MUTATION+73
#define TT_SPIDER_SPINNERS	LAST_CULT_MUTATION+74
#define TT_WINGS_1			LAST_CULT_MUTATION+75
#define TT_WINGS_2			LAST_CULT_MUTATION+76
#define TT_WINGS_3			LAST_CULT_MUTATION+77
#define TT_WINGS_4			LAST_CULT_MUTATION+78
#define TT_WINGS_5			LAST_CULT_MUTATION+79
#define TT_NA_AURA			LAST_CULT_MUTATION+80
#define TT_DR_AURA			LAST_CULT_MUTATION+81
#define TT_MAGIC_BREATHING	LAST_CULT_MUTATION+82
#define TT_ATTRACTIVE_1		LAST_CULT_MUTATION+83
#define TT_ATTRACTIVE_2		LAST_CULT_MUTATION+84
#define TT_FALLEN_ATTRACTIVE	LAST_CULT_MUTATION+85
#define TT_FALLEN_AURA		LAST_CULT_MUTATION+86
#define TT_FALLEN_SCARS		LAST_CULT_MUTATION+87
#define LAST_TIEFLING_TRAIT		TT_FALLEN_SCARS
#define AAT_PRIMINAL			LAST_TIEFLING_TRAIT+1
#define AAT_PRIMINAL_TAIL		LAST_TIEFLING_TRAIT+2
#define	LAST_MUTATION		AAT_PRIMINAL_TAIL

#define MUTATION_LISTSIZE	((LAST_MUTATION-1)/32 + 1)

#define check_mutation(mut) (has_mutation(mut) && (mut <= LAST_CULT_MUTATION || !Upolyd))
#define has_mutation(mut) ((u.mutations[(mut-1)/32]) & (0x1L << ((mut-1)%32)))
#define add_mutation(mut) (u.mutations[(mut-1)/32] |= (0x1L << ((mut-1)%32)))
#define remove_mutation(mut) (u.mutations[(mut-1)/32] &= ~(0x1L << ((mut-1)%32)))

#define MISC_TIEFLING_ABILITY \
	(has_mutation(TT_POISON_CLOUD) || \
	 has_mutation(TT_FIRE_BLAST_1) || \
	 has_mutation(TT_FIRE_BLAST_2) || \
	 has_mutation(TT_COLD_BLAST) || \
	 has_mutation(TT_ACID_BLAST) || \
	 has_mutation(TT_SMOKE) || \
	 has_mutation(TT_COLD_CLOUD) || \
	 has_mutation(TT_SIREN_SONG) || \
	 has_mutation(TT_FROG_CROAK))

#define TIEFLING_GAZE \
	(\
	 has_mutation(TT_HATEFUL_VISION) || \
	 has_mutation(TT_ODD_EYES_1) || \
	 has_mutation(TT_ODD_EYES_2) || \
	 has_mutation(TT_ODD_EYES_3) || \
	 has_mutation(TT_BEHOLDER) || \
	 has_mutation(TT_CANCEL_GAZE) || \
	 has_mutation(TT_MESMERIZING_GAZE))

#define TIEFLING_AUTOGAZE \
	(has_mutation(TT_ODD_EYES_1) || \
	 has_mutation(TT_ODD_EYES_2) || \
	 has_mutation(TT_ODD_EYES_3) || \
	 has_mutation(TT_CANCEL_GAZE) || \
	 has_mutation(TT_HATEFUL_VISION))

#define TIEFLING_AUTOATTACKS \
	(has_mutation(TT_LASHING_TAIL) \
	 || has_mutation(TT_SNAKE_TAIL) \
	 || has_mutation(TT_THIEVING_TAIL) \
	 || has_mutation(AAT_PRIMINAL_TAIL) \
	 || has_mutation(TT_SHADOW_CASTER) \
	 || has_mutation(TT_SHADOW_PAIN) \
	 || has_mutation(TT_SHADOW_SHRED) \
	 || has_mutation(TT_BITING_HAIR) \
	)

#define TIEFLING_AURAS \
	(has_mutation(TT_TEARS_OF_BLOOD) \
	 || has_mutation(TT_FLAMING_HAIR) \
	 || has_mutation(TT_FROSTY_HAIR) \
	)

#define TIEFLING_CLAWS \
	(has_mutation(TT_RAZOR_CLAWS) \
	 || has_mutation(TT_HARD_CLAWS) \
	 || has_mutation(TT_HOOKED_CLAWS) \
	 || has_mutation(TT_TALONS) \
	)

#define TIEFLING_FALLEN \
	(has_mutation(TT_FALLEN_ATTRACTIVE) \
	 || has_mutation(TT_FALLEN_AURA) \
	 || has_mutation(TT_FALLEN_SCARS) \
	)

#define is_horn_mut(mut) \
	(mut == TT_RAMS_HORN \
	 || mut == TT_DEMON_HORN \
	 || mut == TT_UNICORN_HORN \
	 || mut == TT_ANTLERS \
	 || mut == TT_BULL_HORNS \
	)

#define TIEFLING_HORNS \
	(has_mutation(TT_RAMS_HORN) \
	 || has_mutation(TT_DEMON_HORN) \
	 || has_mutation(TT_UNICORN_HORN) \
	 || has_mutation(TT_ANTLERS) \
	 || has_mutation(TT_BULL_HORNS) \
	)

#define TIEFLING_WINGS \
	(has_mutation(TT_WINGS_1) \
	 || has_mutation(TT_WINGS_2) \
	 || has_mutation(TT_WINGS_3) \
	 || has_mutation(TT_WINGS_4) \
	 || has_mutation(TT_WINGS_5) \
	)

#define is_holy_mut(mut) \
	(mut == TT_FALLEN_ATTRACTIVE \
	 || mut == TT_FALLEN_AURA \
	 || mut == TT_FALLEN_SCARS \
	)
#define KNOWS_CURSES (check_mutation(TT_HATEFUL_VISION) || u.seraph_eyes >= SE_CURSES)
#define KNOWS_BLESSINGS (Race_if(PM_AASIMAR) && u.ulevel >= 7)
#define KNOWS_BUC (u.upriest || (KNOWS_CURSES && KNOWS_BLESSINGS))

#define KNOWS_MAGIC (Race_if(PM_INCANTIFIER) || u.seraph_eyes >= SE_MAGIC)

#endif /* MUTATION_H */
