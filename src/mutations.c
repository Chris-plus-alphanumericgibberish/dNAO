#include <math.h>
#include "hack.h"
#include "mattkbp.h"
#include "mutations.h"

STATIC_DCL void mutation_effects(int);
STATIC_DCL void announce_forming_mutation(int);
STATIC_DCL const int *aasimar_gaze_list(void);
STATIC_DCL const int *aasimar_halo_list(long);
STATIC_DCL int aasimar_halo_form_level(long, int *);
STATIC_DCL const int *aasimar_astra_list(void);
STATIC_DCL const int *aasimar_mudra_list(void);
STATIC_DCL const int *aasimar_grace_list(void);
STATIC_DCL int aasimar_grace_form_level(long, int *);
STATIC_DCL int aasimar_mudra_form_level(int *);
STATIC_DCL int archon_aura_mutation(int *);
STATIC_DCL int roll_new_mutation(const int *);
STATIC_DCL int roll_mutation_list(const int *);
STATIC_DCL void check_aasimar_mutations(void);
STATIC_DCL boolean held_or_forming_mutation(int);
STATIC_DCL boolean iksh_na_eye_redundant(int);
STATIC_DCL int pick_iksh_na_eye_mutation(const int *);
STATIC_DCL boolean halo_sunlight(struct monst *);
STATIC_DCL boolean in_hero_light_circle(int, int, int);

/* imported from vision.c, for light-source circles */
extern char circle_data[];
extern int circle_start[];

/* mutation lists */
const int shubbie_mutation_list[] = {ABHORRENT_SPORE,
						 CRAWLING_FLESH,
						 SHUB_RADIANCE,
						 TENDRIL_HAIR,
						 SHIFTING_MIND,
						 SHUB_CLAWS,
						 MIND_STEALER,
						 SHUB_TENTACLES,
						 0
						};

const int yog_mutation_list[] = {YOG_GAZE_1,
					 YOG_GAZE_2,
					 TWIN_MIND,
					 TWIN_DREAMS,
					 TWIN_SAVE,
					 0
					};

const struct mutationtype mutationtypes[] =
{
	{ ABHORRENT_SPORE, -1, "abhorrent spore", "An abhorrent spore has taken root on your body." },
	{ CRAWLING_FLESH, -1, "crawling flesh", "Your flesh crawls, closing wounds with horrid swiftness." },
	{ SHUB_RADIANCE, -1, "illumination of Shub-Nugganoth", "HER light shines in your eyes!" },
	{ TENDRIL_HAIR, -1, "hairlike tendrils", "Your body is covered with hairlike tendrils." },
	{ SHIFTING_MIND, -1, "shifting mind", "Your mind has shifted."},
	{ SHUB_CLAWS, HAND, "pointed claws", "Your fingernails can fuse into your fingers and transform into long claws."},
	{ MIND_STEALER, TONGUE, "mind-stealing tongue", "Your long thin tongue can slip into others' thoughts."},
	{ YOG_GAZE_1, EYE_BP, "eyes of Yog-Sothoth", "Your eyes burn with magenta fire."},
	{ YOG_GAZE_2, EYE_BP, "frenzy of Yog-Sothoth", "Your eyes blaze with magenta fire."},
	{ TWIN_MIND, -1, "chanting tentacles", "Your waist-tentacles chant strange spells."},
	{ TWIN_DREAMS, -1, "mind blasts", "You emit deadly dreams."},
	// { BY_THE_SMELL, -1, "bladder of Yog-Sothoth", "A gas-filled bladder swells then vanishes. An unholy stench fills the air!"},
	{ TWIN_SAVE, -1, "empathic link", "You feel a strong connection to your twin."},
	{ SHUB_TENTACLES, -1, "tentacle valves", "You have valves in your skull, allowing HER to reach through your mind."},
	//Tiefling traits, these use the start forming names and descriptions
	// Smell Traits
	{ TT_POISON_CLOUD, SMELL_TRAIT, "stinking clouds", "You can emit stinking clouds.", "corpse stench", "You stink like a rotting corpse."},
	{ TT_FIRE_BLAST_1, SMELL_TRAIT, "fiery blasts", "You can emit fiery blasts.", "sulfurous smell", "You stink of sulfur."},
	{ TT_FIRE_BLAST_2, SMELL_TRAIT, "firey blasts", "You can emit fiery blasts.", "ashy smell", "You smell of ash and burnt flesh."},
	{ TT_COLD_BLAST, SMELL_TRAIT, "icy blasts", "You can emit icy blasts.", "frosty smell", "You emit an unpleasantly crisp odor that stings the nostrils."},
	{ TT_ACID_BLAST, SMELL_TRAIT, "acidic blasts", "You can emit acidic blasts.", "pungent smell", "You emit a pungent, corrosive odor."},
	// Branch from small scales
	{ TT_NA_SCALES, BODY_SKIN, "brittle scales", "Your skin is covered in hard yet brittle scales.", "small scales", "Your skin is covered in small scales."},
	{ TT_DR_SCALES, BODY_SKIN, "durable scales", "Your skin is covered in durable rubbery scales.", "small scales", "Your skin is covered in small scales."},
	{ TT_SCALES, BODY_SKIN, "metallic scales", "Your skin is covered in metallic scales.", "small scales", "Your skin is covered in small scales."}, 
	// Branch from chitin
	// Fiberous vs. glassy chitin
	{ TT_CHITIN, BODY_SKIN, "chitinous plating", "Your skin is covered in chitinous plating.", "small chitin plates", "Your skin is covered in small chitin plates."},
	// Other skin traits
	{ TT_SLIPPERY_SKIN, BODY_SKIN, "slippery skin", "Your skin is extremely slippery.", "clamy skin", "Your skin is disagreeably clammy."},
	{ TT_FUNGUS_SKIN, BODY_SKIN, "deadly spores", "Deadly spores fly from your skin when struck.", "blotchy skin", "Your skin is covered in blotchy patches."},
	// Branch from long canines
	{ TT_SNAKE_FANGS, MOUTH_TRAIT, "venomous fangs", "You have venomous fangs.", "elongated canines", "Your canines are slightly elongated."},
	{ TT_VAMPIRE_FANGS, MOUTH_TRAIT, "vampiric fangs", "You have blood-sucking fangs.", "elongated canines", "Your canines are slightly elongated."},
	// Branch from bulging cheeks
	{ TT_SPIDER_FANGS, MOUTH_TRAIT, "poisonous chelicerae", "You have poisonous chelicerae tucked inside your mouth.", "bulging cheeks", "Your cheeks bulge slightly."},
	{ TT_SERPENT, MOUTH_TRAIT, "serpentine tongue", "A serpent-coiled second jaw has replaced your tongue.", "bulging cheeks", "Your cheeks bulge slightly."},
	{ TT_BLINDING_VENOM, MOUTH_TRAIT, "blinding venom", "You can spit a blinding venom.", "bulging cheeks", "Your cheeks bulge slightly."},
	// Branch from cloudy breath
	{ TT_SMOKE, WINDPIPE, "smoke breath", "You can exhale a cloud of noxious smoke.", "cloudy breath", "Your breath hangs in the air like white clouds."},
	{ TT_COLD_CLOUD, WINDPIPE, "cold breath", "You can exhale a cloud of freezing mist.", "cloudy breath", "Your breath hangs in the air like white clouds."},
	// Eye traits
	// Branch from writing in eyes
	{ TT_HATEFUL_VISION, EYE_BP, "hateful vision", "You can see curses and the weaknesses of the holy.", "black writing", "The white of your eyes is covered in tiny black script."},
	{ TT_ODD_EYES_1, EYE_BP, "odd eyes", "Your curse-graven gaze can demoralize foes.", "black writing", "The white of your eyes is covered in tiny black script."},
	// Branch from blank white eyes
	{ TT_INFRAVISION_1, EYE_BP, "infravision", "You can see heat signatures.", "blank white eyes", "Your eyes are blank and white."},
	{ TT_EXTRAMISSION_1, EYE_BP, "extramission", "You can see in the dark and light.", "blank white eyes", "Your eyes are blank and white."},
	{ TT_ODD_EYES_2, EYE_BP, "odd eyes", "Your blank gaze can demoralize foes.", "blank white eyes", "Your eyes are blank and white."},
	// Branch from extra eyes
	{ TT_BEHOLDER, EYE_BP, "beholder eyes", "You have extra eyes that emit baleful rays.", "extra eyes", "You have many extra eyes."},
	{ TT_DISCOVERY_1, EYE_BP, "vigilant eyes", "You have extra eyes that can spot hidden things.", "extra eyes", "You have many extra eyes."},
	{ TT_MANY_ODD_EYES, EYE_BP, "odd eyes", "Your multitudinous gaze can demoralize foes.", "extra eyes", "You have many extra eyes."},
	// Branch from glowing eyes
	{ TT_LIGHT, EYE_BP, "light emission", "Your burning eyes light up your surroundings.", "glowing eyes", "Your eyes glow faintly."},
	{ TT_EXTRAMISSION_2, EYE_BP, "extramission", "You can see in the dark and light.", "glowing eyes", "Your eyes glow faintly."},
	{ TT_DISCOVERY_2, EYE_BP, "keen eyes", "You can spot hidden things.", "glowing eyes", "Your eyes glow faintly."},
	{ TT_ODD_EYES_3, EYE_BP, "odd eyes", "Your burning gaze can demoralize foes.", "glowing eyes", "Your eyes glow faintly."},
	// Branch from one large eye
	{ TT_PARALYSIS_GAZE, EYE_BP, "paralysis gaze", "You can paralyze foes with your gaze.", "large eye", "You have one large eye instead of two."},
	{ TT_CANCEL_GAZE, EYE_BP, "cancellation gaze", "You can cancel foes with your gaze.", "large eye", "You have one large eye instead of two."},
	{ TT_PROBING_GAZE, EYE_BP, "probing gaze", "You can examine foes with your gaze.", "large eye", "You have one large eye instead of two."},
	// Misc eye mutations
	{ TT_MESMERIZING_GAZE, EYE_BP, "mesmerizing gaze", "You can mesmerize foes with your gaze.", "swirling irises", "Your irises swirl hypnotically."},
	{ TT_CLOCKWORK_EYES, EYE_BP, "foresight", "Your clockwork eyes can see an instant into the future.", "clockwork eyes", "You have spinning clockwork instead of eyes."},
	{ TT_TEARS_OF_BLOOD, EYE_BP, "tears of blood", "Your foes weep blood in your presence.", "bleeding eyes", "Your eyes weep blood."},
	// Finger traits
	{ TT_COLD_TOUCH, FINGER, "cold touch", "You have a cold touch attack.", "cold fingers", "Your fingers feel cold to the touch."},
	{ TT_DRAIN_TOUCH, FINGER, "drain touch", "You have a life-draining touch attack.", "cold fingers", "Your fingers feel cold to the touch."},
	{ TT_FIRE_TOUCH, FINGER, "fire touch", "You have a fire touch attack.", "warm fingers", "Your fingers feel warm to the touch."},
	{ TT_SHOCK_TOUCH, FINGER, "shock touch", "You have an electric touch attack.", "tingling fingers", "Your fingers tingle slightly."},
	{ TT_EXTRA_FINGERS, FINGER, "enhanced spellcasting", "Your extra fingers improve your spellcasting.", "extra fingers", "You have extra fingers on each hand."},
	{ TT_WEBS, FINGER, "swimming webs", "You can swim.", "webbed fingers", "Your fingers are webbed."},
	// Branching from claws
	{ TT_RAZOR_CLAWS, FINGER, "razor claws", "Your claws are razor sharp.", "small claws", "You have small claws."},
	{ TT_HARD_CLAWS, FINGER, "hard claws", "Your claws are extremely hard.", "small claws", "You have small claws."},
	{ TT_HOOKED_CLAWS, FINGER, "hooked claws", "Your claws are hooked like a bird of prey's.", "small claws", "You have small claws."},
	{ TT_TALONS, FINGER, "talons", "Your have long talons.", "small claws", "You have small claws."},
	// Shadow traits
	{ TT_SHADOW_PAIN, SHADOW_TRAIT, "pain shadow", "Your tortured shadow inflicts agony on foes.", "tortured shadow", "Your shadow writhes and twitches as though in agony."},
	{ TT_SHADOW_SHRED, SHADOW_TRAIT, "shredding shadow", "Your tortured shadow lashes out at foes.", "tortured shadow", "Your shadow writhes and twitches as though in agony."},
	{ TT_WANDERING_SHADOW, SHADOW_TRAIT, "summon shade", "Your shadow can fight alongside you.", "wandering shadow", "Your shadow sometimes wanders away from you."},
	{ TT_SHADOW_CASTER, SHADOW_TRAIT, "shadow casting", "Your shadow can cast spells at foes.", "gesticulating shadow", "Your shadow makes hateful gestures quite on its own."},
	// Blood traits
	{ TT_FIRE_COUNTER, BLOOD, "fiery blood", "Your attackers are burned when they draw your blood.", "warm body", "Your body feels warm to the touch."},
	{ TT_COLD_COUNTER, BLOOD, "icy blood", "Your attackers are chilled when they draw your blood.", "cool body", "Your body feels cool to the touch."},
	{ TT_ACID_COUNTER, BLOOD, "acidic blood", "Your attackers are burned when they draw your blood.", "bilious veins", "Your veins look bilious and unhealthy."},
	{ TT_POISON_COUNTER, BLOOD, "poisonous blood", "Your attackers are poisoned when they draw your blood.", "sickly pallor", "Your skin has a sickly pallor."},
	// Sound traits
	{ TT_FROG_CROAK, SOUND_TRAIT, "deafening croak", "You can emit a deafening croak.", "baggy throat", "Your throat is baggy and frog-like."},
	{ TT_ECHOLOCATION, SOUND_TRAIT, "echolocation", "You can navigate via echolocation.", "bat-like ears", "Your ears are large and bat-like."},
	{ TT_SIREN_SONG, SOUND_TRAIT, "siren song", "You can sing a hypnotic siren song.", "musical voice", "Your voice has a musical quality."},
	// Hair traits
	{ TT_THORN_HAIR, HAIR, "thorny hair", "Your attackers are stabbed by your thorny hair.", "spiky hair", "Your hair is spiky and stiff."},
	{ TT_FLAMING_HAIR, HAIR, "fiery aura", "Your blazing hair burns nearby foes.", "fiery hair", "Your hair blazes with fire."},
	{ TT_FROSTY_HAIR, HAIR, "icy aura", "Your icy hair freezes nearby foes.", "frosty hair", "Your hair is covered in frost."},
	{ TT_BLINDING_HAIR, HAIR, "blinding aura", "Your attackers are blinded by your crystalline hair.", "shimmering hair", "Your hair shimmers with a crystalline sheen."},
	{ TT_BITING_HAIR, HAIR, "biting hair", "Your hair bites at nearby foes.", "writhing hair", "Your hair writhes like a ball of leeches."},
	// Horn traits
	{ TT_RAMS_HORN, HORN_TRAIT, "ram's horns", "You have sturdy ram's horns.", "small horns", "You have small horns."},
	{ TT_DEMON_HORN, HORN_TRAIT, "demon horns", "You have wicked demon horns.", "small horns", "You have small horns."},
	{ TT_UNICORN_HORN, HORN_TRAIT, "diseased horn", "You have a diseased unicorn horn.", "small horn", "You have a small horn."},
	{ TT_ANTLERS, HORN_TRAIT, "antlers", "You have majestic antlers.", "budding antlers", "You have budding antlers."},
	{ TT_BULL_HORNS, HORN_TRAIT, "bull horns", "You have massive bull horns.", "small horns", "You have small horns."},
	// Tail traits
	// Branch from bump at the base of your spine.
	{ TT_PREHENSILE_TAIL, TAIL_TRAIT, "prehensile tail", "You have a prehensile tail.", "tail bud", "You have a bump at the base of your spine."},
	{ TT_LASHING_TAIL, TAIL_TRAIT, "lashing tail", "You have a lashing tail.", "tail bud", "You have a bump at the base of your spine."},
	{ TT_STINGER_TAIL, TAIL_TRAIT, "stinger tail", "You have a stinger-tipped tail.", "tail bud", "You have a bump at the base of your spine."},
	{ TT_SNAKE_TAIL, TAIL_TRAIT, "snake-headed tail", "You have a snake-headed tail.", "tail bud", "You have a bump at the base of your spine."},
	{ TT_THIEVING_TAIL, TAIL_TRAIT, "thieving tail", "You have a thieving tail.", "tail bud", "You have a bump at the base of your spine."},
	{ TT_LIZARD_TAIL, TAIL_TRAIT, "regeneration", "Your lizard's tail regenerates your wounds.", "tail bud", "You have a bump at the base of your spine."},
	{ TT_SPIDER_SPINNERS, TAIL_TRAIT, "silk spinners", "You can spin webs", "tail bud", "You have a bump at the base of your spine."},
	// Wing traits
	{ TT_WINGS_1, WINGS_BP, "flying", "You fly on bat-like wings.", "vestigial wings", "You have tiny bat-like wings."},
	{ TT_WINGS_2, WINGS_BP, "flying", "You fly on black-feathered wings.", "vestigial wings", "You have tiny black-feathered wings."},
	{ TT_WINGS_3, WINGS_BP, "flying", "You fly on white-feathered wings.", "vestigial wings", "You have tiny white-feathered wings."},
	{ TT_WINGS_4, WINGS_BP, "flying", "You fly on red-feathered wings.", "vestigial wings", "You have tiny red-feathered wings."},
	{ TT_WINGS_5, WINGS_BP, "flying", "You fly on giant fly's wings.", "vestigial wings", "You have tiny fly's wings."},
	{ TT_WINGS_6, WINGS_BP, "flying", "You fly on steel-feathered wings.", "vestigial wings", "You have tiny rainbow wings."},
	{ TT_WING_CLAW_1, WINGS_BP, "wing claws", "You have giant wing-like claws.", "vestigial wings", "You have tiny wing-like claws."},
	{ TT_WING_CLAW_2, WINGS_BP, "wing claws", "You have giant wing-like skeletal hands.", "vestigial wings", "You have tiny wing-like skeletal hands."},
	{ TT_WING_CLAW_3, WINGS_BP, "wing stingers", "You have sting-tipped tentacles on your back.", "vestigial wings", "You have tiny sting-tipped tentacles on your back."},
	// Aura traits
	{ TT_NA_AURA, AURA_TRAIT, "unsetling aura", "Your unsettling aura turns away blows.", "unsettling aura", "You have an unsettling aura."},
	{ TT_DR_AURA, AURA_TRAIT, "resistant aura", "Your unsettling aura resists blows.", "unsettling aura", "You have an unsettling aura."},
	{ TT_FALLEN_AURA, AURA_TRAIT, "fallen aura", "Your fallen aura sears both holy and unholy beings.", "unsettling aura", "You have an unsettling aura."},
	// Appearance traits
	{ TT_ATTRACTIVE_1, APPEARANCE_TRAIT, "charming appearance", "You can charm others into giving you gifts.", "attractive appearance", "You are supernaturally attractive."},
	{ TT_ATTRACTIVE_2, APPEARANCE_TRAIT, "disarming appearance", "You can seduce others into disarming themselves.", "attractive appearance", "You are supernaturally attractive."},
	{ TT_FALLEN_ATTRACTIVE, APPEARANCE_TRAIT, "fallen heritage", "The beauty of your fallen ancestry sears both holy and unholy beings.", "attractive appearance", "You are supernaturally attractive."},
	{ TT_FALLEN_SCARS, APPEARANCE_TRAIT, "golden scars", "Your golden scars sear both holy and unholy beings.", "chain-scars", "Your arms and legs bear scars as though from burning chains." },
	// Misc non-bodypart mutations
	{ TT_MAGIC_BREATHING, -1, "magic breathing", "You don't need to breathe.", "drowned appearance", "You look like a drowned corpse."},
	// Aasimar mutations
	{ AAT_PRIMINAL, -1, "gelatinous form", "Your rubbery form allows you to slip through tight spaces and sprout sucking pseudopods from your chest or stomach."},
	{ AAT_PRIMINAL_TAIL, TAIL_TRAIT, "prehensile thieving tail", "You have a thieving, prehensile tail."},
	// Archon eyes
	{ AAT_EXTRAMISSION_1, EYE_BP, "extramission", "You can see in the dark and light.", "holy eyes", "You have rings of light in your eyes."},
	{ AAT_BLESSINGS_1, EYE_BP, "see blessings", "You can see blessings and curses.", "holy eyes", "You have rings of light in your eyes."},
	{ AAT_INVIS_1, EYE_BP, "see invisibility and shape", "You can see invisible creatures and discern the true form of shape-changers.", "holy eyes", "You have rings of light in your eyes."},
	{ AAT_HUNT_UNHOLY_1, EYE_BP, "hunt unholy", "You demoralize and hunt the unholy.", "holy eyes", "You have rings of light in your eyes."},
	{ AAT_HUNT_UNORDERED, EYE_BP, "hunt unordered", "You demoralize and hunt the unordered.", "holy eyes", "You have rings of light in your eyes."},
	// Deva eyes
	{ AAT_DEVA_SIGHT, EYE_BP, "deva sight", "You have the sight of the devas.", "holy eyes", "You have an extra eye in your brow."},
	{ AAT_IKSH_NA_SIGHT, EYE_BP, "deva sight", "You have the sight of the devas.", "holy eyes", "You have an extra eye in your brow."},
	{ AAT_BLESSINGS_2, EYE_BP, "see blessings", "You can see blessings and curses.", "holy eyes", "You have an extra eye in your brow."},
	{ AAT_INVIS_2, EYE_BP, "see invisibility and shape", "You can see invisible creatures and discern the true form of shape-changers.", "holy eyes", "You have an extra eye in your brow."},
	{ AAT_HUNT_UNHOLY_2, EYE_BP, "hunt unholy", "You demoralize and hunt the unholy.", "holy eyes", "You have an extra eye in your brow."},
	{ AAT_HUNT_UNBALANCED, EYE_BP, "hunt unbalanced", "You demoralize and hunt the unbalanced.", "holy eyes", "You have an extra eye in your brow."},
	// Eladrin eyes
	{ AAT_ELADRIN_SIGHT, EYE_BP, "eladrin sight", "You have the sight of the eladrin.", "holy eyes", "You have flecks of light in your eyes."},
	{ AAT_BLESSINGS_3, EYE_BP, "see blessings", "You can see blessings and curses.", "holy eyes", "You have flecks of light in your eyes."},
	{ AAT_HUNT_UNHOLY_3, EYE_BP, "hunt unholy", "You demoralize and hunt the unholy.", "holy eyes", "You have flecks of light in your eyes."},
	{ AAT_HUNT_TYRANTS, EYE_BP, "hunt tyrants", "You demoralize and hunt the tyrants.", "holy eyes", "You have flecks of light in your eyes."},
	{ AAT_ELADRIN_GAZE, EYE_BP, "eladrin gaze", "You have the glare of an eladrin lord.", "holy eyes", "You have flecks of light in your eyes."},
	// Cloudface eyes
	{ AAT_CLOUDFACE_INFRAVISION, EYE_BP, "far infravision", "You can see warm-blooded creatures, even through walls.", "ring of light", "You have a faint ring of light in front of your face."},
	{ AAT_CLOUDFACE_BLOODSENSE, EYE_BP, "far bloodsense", "You can sense the blood of creatures, even through walls.", "ring of light", "You have a faint ring of light in front of your face."},
	{ AAT_CLOUDFACE_LIFESENSE, EYE_BP, "far lifesense", "You can sense living creatures, even through walls.", "ring of light", "You have a faint ring of light in front of your face."},
	{ AAT_INVIS_3, EYE_BP, "see invisibility and shape", "You can see invisible creatures and discern the true form of shape-changers.", "ring of light", "You have a faint ring of light in front of your face."},
	{ AAT_BLESSINGS_4, EYE_BP, "see blessings", "You can see blessings and curses.", "ring of light", "You have a faint ring of light in front of your face."},
	{ AAT_EXTRAMISSION_2, EYE_BP, "extramission", "You can see in the dark and light.", "ring of light", "You have a faint ring of light in front of your face."},
	// Archon halo
	{ AAT_HALO_PURIFY, AURA_TRAIT, "purifying halo", "Your halo's light purifies the unholy.", "halo", "Your halo shines brightly."},
	{ AAT_HALO_CLEANSE, AURA_TRAIT, "cleansing halo", "Your halo's light cleanses the undead.", "halo", "Your halo shines brightly."},
	{ AAT_HALO_ORDER, AURA_TRAIT, "ordering halo", "Your halo's light brings order to chaos.", "halo", "Your halo shines brightly."},
	{ AAT_HALO_DAZZLING, AURA_TRAIT, "dazzling halo", "Your halo's light dazzles aggressors.", "halo", "Your halo shines brightly."},
	{ AAT_HALO_PROTECTIVE, AURA_TRAIT, "protective halo", "Your halo's light protects from harm.", "halo", "Your halo shines brightly."},
	{ AAT_HALO_DISPEL, AURA_TRAIT, "dispel halo", "Your halo's light dispels black magic.", "halo", "Your halo shines brightly."},
	// Solar halo
	{ AAT_HALO_DAWN_PURIFY, AURA_TRAIT, "purifying dawn halo", "Your halo shines with the purifying light of dawn.", "halo", "Your halo shines brightly."},
	{ AAT_HALO_DAWN_FIRE, AURA_TRAIT, "fiery dawn halo", "Your halo burns with the fires of dawn.", "halo", "Your halo shines brightly."},
	{ AAT_HALO_DAWN_CLEANSE, AURA_TRAIT, "cleansing dawn halo", "Your halo shines with the cleansing light of dawn.", "halo", "Your halo shines brightly."},
	{ AAT_HALO_DAWN_HEAL, AURA_TRAIT, "healing dawn halo", "Your halo shines with the healing light of dawn.", "halo", "Your halo shines brightly."},
	// Light archon halo
	{ AAT_HALO_SILVER, AURA_TRAIT, "silver halo", "Your halo shines silver.", "halo", "Your halo shines brightly."},
	// Cloudface halos
	{ AAT_FACE_PURIFY, AURA_TRAIT, "purifying face", "The light of your face purifies the unholy.", "halo", "Your face shines brightly."},
	{ AAT_FACE_CLEANSE, AURA_TRAIT, "cleansing face", "The light of your face cleanses the undead.", "halo", "Your face shines brightly."},
	{ AAT_FACE_FREEDOM, AURA_TRAIT, "freeing face", "The light of your face brings freedom from tyrants.", "halo", "Your face shines brightly."},
	{ AAT_FACE_DAZZLING, AURA_TRAIT, "dazzling face", "The light of your face dazzles aggressors.", "halo", "Your face shines brightly."},
	{ AAT_FACE_PROTECTIVE, AURA_TRAIT, "protective face", "The light of your face protects from harm.", "halo", "Your face shines brightly."},
	{ AAT_FACE_DISPEL, AURA_TRAIT, "dispel face", "The light of your face dispels black magic.", "halo", "Your face shines brightly."},
	{ AAT_FACE_HEAL, AURA_TRAIT, "healing face", "The light of your face heals the wounded.", "halo", "Your face shines brightly."},
	{ AAT_FACE_FIRE, AURA_TRAIT, "fiery face", "The light of your face burns like fire.", "halo", "Your face shines brightly."},
	// Archon aura
	//  Early auras
	{ AAT_SHIELD_AURA, AURA_TRAIT, "shield aura", "Your aura suffuses your shield.", "aura", "You have a righteous aura."},
	{ AAT_TRUMPET_AURA, AURA_TRAIT, "trumpet aura", "Your aura warns allies of impending danger.", "aura", "You have a righteous aura."},
	{ AAT_PANAKEIAN_AURA, AURA_TRAIT, "panakeian aura", "Your aura brings allies good health.", "aura", "You have a righteous aura."},
	//  Late auras
	{ AAT_JUSTICE_AURA, AURA_TRAIT, "justice aura", "Your aura brings justice to your foes.", "aura", "You have a righteous aura."},
	{ AAT_SWORD_AURA, AURA_TRAIT, "sword aura", "Your aura empowers your allies' attacks.", "aura", "You have a righteous aura."},
	{ AAT_WARDEN_AURA, AURA_TRAIT, "warden aura", "Your aura heals your allies.", "aura", "You have a righteous aura."},
	// Deva astras
	{ AAT_MOVANIC_ASTRA, AURA_TRAIT, "movanic astra", "Your allies' weapons burn with mystic fire.", "astra", "You have a righteous aura."},
	{ AAT_MONADIC_ASTRA, AURA_TRAIT, "monadic astra", "Your allies' weapons slay the undead and their glares bring destruction.", "astra", "You have a righteous aura."},
	{ AAT_ASTRAL_ASTRA, AURA_TRAIT, "astral astra", "Your allies' weapons strike with stunning force.", "astra", "You have a righteous aura."},
	{ AAT_SURYA_ASTRA, AURA_TRAIT, "surya astra", "Your allies shoot slaying arrows.", "astra", "You have a righteous aura."},
	// Priminal astras
	{ AAT_ACIDIC_ASTRA, AURA_TRAIT, "acidic astra", "Your allies' weapons burn with clear acid.", "astra", "You have a righteous aura."},
	{ AAT_WATERY_ASTRA, AURA_TRAIT, "watery astra", "Your allies' weapons weep living water.", "astra", "You have a righteous aura."},
	// Deva mudras
	{ AAT_FEARLESSNESS_MUDRA, AURA_TRAIT, "fearlessness mudra", "A pair of your aura's arms make the gesture of fearlessness.", "mudra", "You have an aura of many arms."},
	{ AAT_MEDITATION_MUDRA, AURA_TRAIT, "meditation mudra", "A pair of your aura's arms make the gesture of meditation.", "mudra", "You have an aura of many arms."},
	{ AAT_CLARITY_MUDRA, AURA_TRAIT, "clarity mudra", "A pair of your aura's arms make the gesture of clarity.", "mudra", "You have an aura of many arms."},
	{ AAT_THREATENING_MUDRA, AURA_TRAIT, "threatening mudra", "A pair of your aura's arms make the gesture of banishing demons.", "mudra", "You have an aura of many arms."},
	// Priminal mudras (deva mudras, made with pseudopods once the hero is gelatinous)
	{ AAT_PRIMINAL_FEARLESSNESS_MUDRA, AURA_TRAIT, "fearlessness mudra", "A pair of your pseudopods make the gesture of fearlessness.", "mudra", "You have many pseudopods."},
	{ AAT_PRIMINAL_MEDITATION_MUDRA, AURA_TRAIT, "meditation mudra", "A pair of your pseudopods make the gesture of meditation.", "mudra", "You have many pseudopods."},
	{ AAT_PRIMINAL_CLARITY_MUDRA, AURA_TRAIT, "clarity mudra", "A pair of your pseudopods make the gesture of clarity.", "mudra", "You have many pseudopods."},
	{ AAT_PRIMINAL_THREATENING_MUDRA, AURA_TRAIT, "threatening mudra", "A pair of your pseudopods make the gesture of banishing demons.", "mudra", "You have many pseudopods."},
	// Eladrin grace
	{ AAT_GRACE_TRAPS, -1, "graceful trapavoid", "Your grace allows you to avoid traps.", "grace", "You move with graceful ease."},
	{ AAT_GRACE_DODGE, -1, "graceful dodge", "Your grace allows you to dodge attacks.", "grace", "You move with graceful ease."},
	{ AAT_GRACE_SWIFT, -1, "swift grace", "Your grace allows you to move swiftly.", "grace", "You move with graceful ease."},
	{ AAT_GRACE_PRECISION, -1, "precise grace", "Your grace allows you to strike with precision.", "grace", "You move with graceful ease."},
	{ AAT_GRACE_MANTLE, AURA_TRAIT, "mantle of grace", "Your graceful form is mantled by your power.", "grace", "You move with graceful ease."},
	{ AAT_GRACE_BLADES, -1, "graceful blade-dance", "Your grace makes war a dance of blades.", "grace", "You move with graceful ease."},
	{ AAT_GRACE_DANCE, -1, "graceful dance", "Your grace makes war a dance.", "grace", "You move with graceful ease."},
	{ AAT_GRACE_EXTRA, -1, "graceful extra-attack", "Your grace allows you to strike twice in a single motion.", "grace", "You move with graceful ease."},
	{ AAT_GRACE_STRIKE, -1, "graceful strike", "Your grace allows you to strike with frightful speed.", "grace", "You move with graceful ease."},
	{ AAT_GRACE_POWER, -1, "graceful power", "Your grace allows you to strike with breathtaking power.", "grace", "You move with graceful ease."},
	{ AAT_GRACE_GIFTS, -1, "grace-inspired gifts", "Your grace can cause your foes to throw their blades at your feet.", "grace", "You move with graceful ease."},
	{ AAT_GRACE_DEFLECT, -1, "graceful deflect", "Your grace allows you to deflect arrows.", "grace", "You move with graceful ease."},
	{ AAT_GRACE_DEFLECT_SPELLS, -1, "graceful spell-parry", "Your grace allows you to deflect spells.", "grace", "You move with graceful ease."},
	{ AAT_GRACE_PARRY, -1, "graceful parry", "Your grace allows you to parry attacks with ease.", "grace", "You move with graceful ease."},
	{ AAT_GRACE_LUNGE, -1, "graceful lunge", "Your grace allows you to make unexpected lunges.", "grace", "You move with graceful ease."},
	{ AAT_GRACE_CRYSTAL, -1, "crystal synergy", "Your grace allows you to harness the power of crystal arms and armor.", "grace", "You move with graceful ease."},
	// End marker
	{ 0 },
};

void
confer_mutation(mutation)
int mutation;
{
	int i;
	if(mutation == SHIFTING_MIND){
		reset_skills();
	}
	else add_mutation(mutation);
	
	mutation_effects(mutation);

	if(mutation <= LAST_CULT_MUTATION || !Upolyd) for (i = 0; mutationtypes[i].mutation; i++){
		if(mutation == mutationtypes[i].mutation){
			pline("%s", mutationtypes[i].description);
		}
	}

	if(mutation == SHUB_RADIANCE){
		calc_total_maxhp();
		calc_total_maxen();
	}
}

boolean
any_mutation()
{
	for(int i = 0; i < MUTATION_LISTSIZE; i++)
		if(u.mutations[i])
			return TRUE;
	if(u.next_tiefling_mutation)
		return TRUE;
	if(u.next_aasimar_mutation)
		return TRUE;
	return FALSE;
}

/* Eye mutation lists (0-terminated) used by iksh_na_eye_blocks[] */
static const int iksh_na_odd_eyes_grp[] = {
	TT_ODD_EYES_1, TT_ODD_EYES_2, TT_ODD_EYES_3, TT_MANY_ODD_EYES, 0
};
static const int iksh_na_discovery_grp[] = { TT_DISCOVERY_1, TT_DISCOVERY_2, 0 };
static const int iksh_na_blessings_grp[] = { AAT_BLESSINGS_1, AAT_BLESSINGS_2, AAT_BLESSINGS_3, 0 };
static const int iksh_na_hunt_unholy_grp[] = { AAT_HUNT_UNHOLY_1, AAT_HUNT_UNHOLY_2, AAT_HUNT_UNHOLY_3, 0 };
static const int iksh_na_see_invis_grp[] = { AAT_IKSH_NA_SIGHT, AAT_INVIS_1, AAT_INVIS_2, 0 };
static const int iksh_na_extramission_grp[] = { TT_EXTRAMISSION_1, TT_EXTRAMISSION_2, AAT_EXTRAMISSION_1, 0 };
/* catsight, infravision, and lifesense, each made redundant by extramission */
static const int iksh_na_extramission_redundant[] = { AAT_ELADRIN_SIGHT, TT_INFRAVISION_1, AAT_IKSH_NA_SIGHT, 0 };
static const int iksh_na_eladrin_sight[] = { AAT_ELADRIN_SIGHT, 0 };
static const int iksh_na_infravision[] = { TT_INFRAVISION_1, 0 };

/* An Iksh'na holding or forming any mutation in held can't develop any mutation in blocked */
struct iksh_na_eye_block {
	const int *held;
	const int *blocked;
};
static const struct iksh_na_eye_block iksh_na_eye_blocks[] = {
	{ iksh_na_odd_eyes_grp, iksh_na_odd_eyes_grp },
	{ iksh_na_discovery_grp, iksh_na_discovery_grp },
	{ iksh_na_blessings_grp, iksh_na_blessings_grp },
	{ iksh_na_hunt_unholy_grp, iksh_na_hunt_unholy_grp },
	{ iksh_na_see_invis_grp, iksh_na_see_invis_grp },
	{ iksh_na_extramission_grp, iksh_na_extramission_grp },
	{ iksh_na_extramission_grp, iksh_na_extramission_redundant },
	{ iksh_na_extramission_redundant, iksh_na_extramission_grp },
	/* eladrin sight already grants infravision; infravision doesn't block eladrin sight's catsight */
	{ iksh_na_eladrin_sight, iksh_na_infravision },
	{ (const int *)0, (const int *)0 }
};

STATIC_OVL boolean
held_or_forming_mutation(int mut)
{
	return has_mutation(mut) || u.next_tiefling_mutation == mut || u.next_aasimar_mutation == mut;
}

/* Return TRUE if an Iksh'na developing eye mutation mut would duplicate, or be
 * made redundant by, a mutation the hero holds or is forming.
 */
STATIC_OVL boolean
iksh_na_eye_redundant(int mut)
{
	if(held_or_forming_mutation(mut))
		return TRUE;
	for(int b = 0; iksh_na_eye_blocks[b].held; b++){
		const int *held = iksh_na_eye_blocks[b].held;
		const int *blocked = iksh_na_eye_blocks[b].blocked;
		boolean is_blocked = FALSE, any_held = FALSE;
		for(int k = 0; blocked[k]; k++)
			if(blocked[k] == mut){ is_blocked = TRUE; break; }
		if(!is_blocked)
			continue;
		for(int k = 0; held[k]; k++)
			if(held_or_forming_mutation(held[k])){ any_held = TRUE; break; }
		if(any_held)
			return TRUE;
	}
	return FALSE;
}

/* Return a random non-redundant eye mutation from the 0-terminated list candidates, or 0 if there is none. */
STATIC_OVL int
pick_iksh_na_eye_mutation(const int *candidates)
{
	int pool[LAST_MUTATION];
	int count = 0;
	if(!candidates)
		return 0;
	for(int i = 0; candidates[i]; i++)
		if(!iksh_na_eye_redundant(candidates[i]))
			pool[count++] = candidates[i];
	return count ? pool[rn2(count)] : 0;
}

void
init_iksh_na_mutations()
{
	static const int large_eye[] = {
		TT_PARALYSIS_GAZE, TT_CANCEL_GAZE, TT_PROBING_GAZE
	};
	u.next_tiefling_mutation = ROLL_FROM(large_eye);
	u.next_mutation_level = 7;
}

void
check_iksh_na_mutations()
{
	int just_completed = u.next_mutation_level;
	if(u.next_tiefling_mutation && u.ulevel >= just_completed){
		confer_mutation(u.next_tiefling_mutation);
		u.next_tiefling_mutation = 0;
	} else {
		return;
	}
	int chosen = 0;
	if(just_completed == 7){
		static const int extra_eye[] = {
			TT_BEHOLDER, TT_DISCOVERY_1, TT_MANY_ODD_EYES, 0
		};
		chosen = pick_iksh_na_eye_mutation(extra_eye);
		u.next_mutation_level = 14;
	} else if(just_completed == 14){
		static const int exclude[] = {
			//Bad flavor
			TT_HATEFUL_VISION, TT_ODD_EYES_1,
			TT_CLOCKWORK_EYES, TT_TEARS_OF_BLOOD,
			//Bad effect
			TT_LIGHT,
			//Not a second single-eye mutation
			TT_PARALYSIS_GAZE, TT_CANCEL_GAZE, TT_PROBING_GAZE,
		};
		int candidates[LAST_MUTATION + 1];
		int count = 0;
		for(int j = 0; mutationtypes[j].mutation; j++){
			int mut = mutationtypes[j].mutation;
			if(mutationtypes[j].bodypart != EYE_BP) continue;
			if(mut <= LAST_CULT_MUTATION) continue;
			if(mut > LAST_TIEFLING_TRAIT) continue;
			boolean skip = FALSE;
			for(int k = 0; k < SIZE(exclude); k++)
				if(mut == exclude[k]){ skip = TRUE; break; }
			if(skip) continue;
			candidates[count++] = mut;
		}
		candidates[count] = 0;
		chosen = pick_iksh_na_eye_mutation(candidates);
		if(chosen)
			u.next_mutation_level = 21;
	}
	/* just_completed == 21: sequence complete, chosen stays 0 */
	if(chosen){
		u.next_tiefling_mutation = chosen;
		if(!Upolyd){
			for(int j = 0; mutationtypes[j].mutation; j++){
				if(mutationtypes[j].mutation == chosen){
					pline("%s", mutationtypes[j].start_forming);
					break;
				}
			}
		}
	}
}

/* Return TRUE if mon is opposed to the hero's holiness (neutral counts as holy). */
boolean
opposes_u_holiness(struct monst *mon)
{
	return (u_holiness() == UNHOLY_HOLINESS) ? is_holy_monster(mon) : is_unholy_monster(mon);
}

/* Return TRUE if one of the hero's aasimar hunting gazes targets mdef. */
boolean
aasimar_hunts(struct monst *mdef)
{
	if((check_mutation(AAT_HUNT_UNHOLY_1) || check_mutation(AAT_HUNT_UNHOLY_2) || check_mutation(AAT_HUNT_UNHOLY_3))
		&& opposes_u_holiness(mdef)
	)
		return TRUE;
	if(check_mutation(AAT_HUNT_UNORDERED) && !is_lawful_mon(mdef))
		return TRUE;
	if(check_mutation(AAT_HUNT_UNBALANCED) && !is_neutral_mon(mdef))
		return TRUE;
	if(check_mutation(AAT_HUNT_TYRANTS) && !is_chaotic_mon(mdef))
		return TRUE;
	return FALSE;
}

/* Return TRUE if the hero's eladrin subtype has a gaze attack (that of the
 * matching eladrin monster), filling in *gaze with it if gaze is non-null.
 */
boolean
eladrin_gaze_attack(struct attack *gaze)
{
	struct attack eladrin_gaze = {AT_GAZE, AD_PHYS, 0, 0, .bodypart = ATKBP(EYES)};

	if(flags.aasimar_type != AASIMAR_TYPE_ELADRIN)
		return FALSE;
	switch(flags.aasimar_subtype){
		case AASIMAR_SUBTYPE_FIRRE:
			eladrin_gaze.adtyp = AD_FIRE;
			eladrin_gaze.damn = 1;
			eladrin_gaze.damd = 10;
			break;
		case AASIMAR_SUBTYPE_GHAELE:
			eladrin_gaze.adtyp = AD_CONF;
			eladrin_gaze.damn = 2;
			eladrin_gaze.damd = 10;
			break;
		case AASIMAR_SUBTYPE_TULANI:
			eladrin_gaze.adtyp = AD_STUN;
			eladrin_gaze.damn = 2;
			eladrin_gaze.damd = 10;
			break;
		default:
			return FALSE;
	}
	if(gaze)
		*gaze = eladrin_gaze;
	return TRUE;
}

/* Print the "starting to form" message for mutation mut, unless polymorphed. */
STATIC_OVL void
announce_forming_mutation(int mut)
{
	if(Upolyd)
		return;
	for(int j = 0; mutationtypes[j].mutation; j++){
		if(mutationtypes[j].mutation == mut){
			if(mutationtypes[j].start_forming)
				pline("%s", mutationtypes[j].start_forming);
			break;
		}
	}
}

/* Return the 0-terminated list of gaze mutations the hero's aasimar type can
 * develop, or null if the hero's aasimar type doesn't develop one.
 */
STATIC_OVL const int *
aasimar_gaze_list(void)
{
	static const int archon_gazes[] = {
		AAT_EXTRAMISSION_1, AAT_BLESSINGS_1, AAT_INVIS_1, AAT_HUNT_UNHOLY_1, AAT_HUNT_UNORDERED, 0
	};
	static const int deva_gazes[] = {
		AAT_DEVA_SIGHT, AAT_BLESSINGS_2, AAT_INVIS_2, AAT_HUNT_UNHOLY_2, AAT_HUNT_UNBALANCED, 0
	};
	static const int eladrin_gazes[] = {
		AAT_ELADRIN_SIGHT, AAT_BLESSINGS_3, AAT_HUNT_UNHOLY_3, AAT_HUNT_TYRANTS, 0
	};
	/* subtypes whose eladrin have a gaze attack */
	static const int gazing_eladrin_gazes[] = {
		AAT_ELADRIN_SIGHT, AAT_BLESSINGS_3, AAT_HUNT_UNHOLY_3, AAT_HUNT_TYRANTS, AAT_ELADRIN_GAZE, 0
	};
	/* cloudfaces already have their unsettling gaze, so theirs are all sight */
	static const int cloudface_gazes[] = {
		AAT_CLOUDFACE_INFRAVISION, AAT_CLOUDFACE_BLOODSENSE, AAT_CLOUDFACE_LIFESENSE,
		AAT_INVIS_3, AAT_BLESSINGS_4, AAT_EXTRAMISSION_2, 0
	};
	static const int iksh_na_gazes[] = { AAT_IKSH_NA_SIGHT, AAT_BLESSINGS_2, AAT_INVIS_2, AAT_HUNT_UNHOLY_2, AAT_HUNT_UNBALANCED, 
		AAT_ELADRIN_SIGHT, AAT_BLESSINGS_3, AAT_HUNT_UNHOLY_3, AAT_HUNT_TYRANTS,
		0 };

	if(!RACE_IF_AASIMAR)
		return (const int *)0;
	switch(flags.aasimar_type){
		case AASIMAR_TYPE_ARCHON:
			return archon_gazes;
		case AASIMAR_TYPE_DEVA:
		case AASIMAR_TYPE_SURYA:
		case AASIMAR_TYPE_PRIMINAL:
			return deva_gazes;
		case AASIMAR_TYPE_ELADRIN:
			return eladrin_gaze_attack((struct attack *)0) ? gazing_eladrin_gazes : eladrin_gazes;
		case AASIMAR_TYPE_IKSH_NA:
			return iksh_na_gazes;
		case AASIMAR_TYPE_CLOUDFACE:
			return cloudface_gazes;
	}
	return (const int *)0;
}

/* Return the 0-terminated list of halo mutations the hero's aasimar type can
 * develop at halo stage (AAS_STAGE_HALO, AAS_STAGE_HALO_2, or AAS_STAGE_HALO_3),
 * or null if the hero's aasimar type doesn't develop one at that stage.
 */
STATIC_OVL const int *
aasimar_halo_list(long stage)
{
	static const int archon_halos[] = {
		AAT_HALO_PURIFY, AAT_HALO_CLEANSE, AAT_HALO_ORDER, AAT_HALO_DAZZLING, AAT_HALO_PROTECTIVE, AAT_HALO_DISPEL, 0
	};
	static const int seraph_halos[] = {
		AAT_HALO_PURIFY, AAT_HALO_CLEANSE, AAT_HALO_ORDER, AAT_HALO_DAZZLING, AAT_HALO_PROTECTIVE, AAT_HALO_DISPEL,
		AAT_HALO_SILVER, 0
	};
	/* the archon halos that don't overlap with the dawn halos */
	static const int surya_archon_halos[] = {
		AAT_HALO_ORDER, AAT_HALO_DAZZLING, AAT_HALO_PROTECTIVE, AAT_HALO_DISPEL, 0
	};
	static const int surya_dawn_halos[] = {
		AAT_HALO_DAWN_PURIFY, AAT_HALO_DAWN_FIRE, AAT_HALO_DAWN_CLEANSE, AAT_HALO_DAWN_HEAL, 0
	};
	static const int cloudface_halos[] = {
		AAT_FACE_PURIFY, AAT_FACE_CLEANSE, AAT_FACE_FREEDOM, AAT_FACE_DAZZLING,
		AAT_FACE_PROTECTIVE, AAT_FACE_DISPEL, AAT_FACE_HEAL, AAT_FACE_FIRE, 0
	};

	if(!RACE_IF_AASIMAR)
		return (const int *)0;
	switch(flags.aasimar_type){
		case AASIMAR_TYPE_ARCHON:
			return archon_halos;
		case AASIMAR_TYPE_SERAPH:
			return seraph_halos;
		case AASIMAR_TYPE_SURYA:
			return stage == AAS_STAGE_HALO ? surya_archon_halos
				: stage == AAS_STAGE_HALO_2 ? surya_dawn_halos
				: (const int *)0;
		case AASIMAR_TYPE_CLOUDFACE:
			return stage == AAS_STAGE_HALO ? cloudface_halos : (const int *)0;
	}
	return (const int *)0;
}

/* Return the experience level at which halo stage (AAS_STAGE_HALO,
 * AAS_STAGE_HALO_2, or AAS_STAGE_HALO_3) begins forming for the hero's aasimar
 * type, setting *activate_level to the level at which it activates, or 0 if the
 * hero's aasimar type doesn't have that stage.
 */
STATIC_OVL int
aasimar_halo_form_level(long stage, int *activate_level)
{
	int form_level = 0;

	if(!RACE_IF_AASIMAR)
		return 0;
	switch(flags.aasimar_type){
		case AASIMAR_TYPE_ARCHON:
			form_level = stage == AAS_STAGE_HALO ? 14 : 0;
			break;
		case AASIMAR_TYPE_SURYA:
			if(stage == AAS_STAGE_HALO){
				*activate_level = 19;
				return 15;
			}
			if(stage == AAS_STAGE_HALO_2){
				*activate_level = 30;
				return 26;
			}
			break;
		case AASIMAR_TYPE_SERAPH:
			form_level = stage == AAS_STAGE_HALO ? 7
				: stage == AAS_STAGE_HALO_2 ? 14
				: stage == AAS_STAGE_HALO_3 ? 21
				: 0;
			break;
		case AASIMAR_TYPE_CLOUDFACE:
			if(stage == AAS_STAGE_HALO){
				*activate_level = 24;
				return 21;
			}
			break;
	}
	*activate_level = form_level + 3;
	return form_level;
}

/* Return the 0-terminated list of astra mutations the hero's aasimar type can
 * develop, or null if the hero's aasimar type doesn't develop one.
 */
STATIC_OVL const int *
aasimar_astra_list(void)
{
	static const int deva_astras[] = {
		AAT_MOVANIC_ASTRA, AAT_MONADIC_ASTRA, AAT_ASTRAL_ASTRA, AAT_SURYA_ASTRA, 0
	};
	static const int priminal_astras[] = {
		AAT_MOVANIC_ASTRA, AAT_MONADIC_ASTRA, AAT_ASTRAL_ASTRA, AAT_SURYA_ASTRA,
		AAT_ACIDIC_ASTRA, AAT_WATERY_ASTRA, 0
	};

	if(!RACE_IF_AASIMAR)
		return (const int *)0;
	switch(flags.aasimar_type){
		case AASIMAR_TYPE_DEVA:
		case AASIMAR_TYPE_SURYA:
			return deva_astras;
		case AASIMAR_TYPE_PRIMINAL:
			return priminal_astras;
	}
	return (const int *)0;
}

/* Return the 0-terminated list of mudra mutations the hero's aasimar type can
 * develop, or null if the hero's aasimar type doesn't develop one.
 */
STATIC_OVL const int *
aasimar_mudra_list(void)
{
	static const int deva_mudras[] = {
		AAT_FEARLESSNESS_MUDRA, AAT_MEDITATION_MUDRA, AAT_CLARITY_MUDRA, AAT_THREATENING_MUDRA, 0
	};
	static const int priminal_mudras[] = {
		AAT_PRIMINAL_FEARLESSNESS_MUDRA, AAT_PRIMINAL_MEDITATION_MUDRA, AAT_PRIMINAL_CLARITY_MUDRA, AAT_PRIMINAL_THREATENING_MUDRA, 0
	};

	if(!RACE_IF_AASIMAR)
		return (const int *)0;
	switch(flags.aasimar_type){
		case AASIMAR_TYPE_DEVA:
		case AASIMAR_TYPE_IKSH_NA:
			return deva_mudras;
		case AASIMAR_TYPE_PRIMINAL:
			/* gelatinous priminals make their mudras with pseudopods */
			return has_mutation(AAT_PRIMINAL) ? priminal_mudras : deva_mudras;
	}
	return (const int *)0;
}

/* Return the experience level at which the hero's aasimar type begins forming
 * its mudra, setting *activate_level to the level at which it activates, or 0
 * if the hero's aasimar type doesn't develop one.
 */
STATIC_OVL int
aasimar_mudra_form_level(int *activate_level)
{
	if(!RACE_IF_AASIMAR)
		return 0;
	switch(flags.aasimar_type){
		case AASIMAR_TYPE_DEVA:
		case AASIMAR_TYPE_PRIMINAL:
			*activate_level = 19;
			return 15;
		case AASIMAR_TYPE_IKSH_NA:
			*activate_level = 30;
			return 27;
	}
	return 0;
}

/* Return the 0-terminated list of grace mutations the hero's aasimar type can
 * develop, or null if the hero's aasimar type doesn't develop one.
 */
STATIC_OVL const int *
aasimar_grace_list(void)
{
	static const int eladrin_graces[] = {
		AAT_GRACE_TRAPS, AAT_GRACE_DODGE, AAT_GRACE_SWIFT, AAT_GRACE_PRECISION, AAT_GRACE_MANTLE,
		AAT_GRACE_BLADES, AAT_GRACE_DANCE, AAT_GRACE_EXTRA,
		AAT_GRACE_STRIKE, AAT_GRACE_POWER, AAT_GRACE_GIFTS,
		AAT_GRACE_DEFLECT, AAT_GRACE_DEFLECT_SPELLS, AAT_GRACE_PARRY,
		AAT_GRACE_LUNGE, AAT_GRACE_CRYSTAL, 0
	};

	if(!RACE_IF_AASIMAR)
		return (const int *)0;
	switch(flags.aasimar_type){
		case AASIMAR_TYPE_ELADRIN:
			return eladrin_graces;
	}
	return (const int *)0;
}

/* Return the experience level at which grace stage (AAS_STAGE_GRACE,
 * AAS_STAGE_GRACE_2, or AAS_STAGE_GRACE_3) begins forming for the hero's aasimar
 * type, setting *activate_level to the level at which it activates, or 0 if the
 * hero's aasimar type doesn't have that stage.
 */
STATIC_OVL int
aasimar_grace_form_level(long stage, int *activate_level)
{
	if(!RACE_IF_AASIMAR || flags.aasimar_type != AASIMAR_TYPE_ELADRIN)
		return 0;
	/* Coure and Gae have their always-on auto-attacks in place of the first grace */
	if(stage == AAS_STAGE_GRACE
		&& (flags.aasimar_subtype == AASIMAR_SUBTYPE_COURE || flags.aasimar_subtype == AASIMAR_SUBTYPE_GAE)
	)
		return 0;
	if(stage == AAS_STAGE_GRACE){
		*activate_level = 14;
		return 11;
	}
	if(stage == AAS_STAGE_GRACE_2){
		*activate_level = 20;
		return 17;
	}
	if(stage == AAS_STAGE_GRACE_3){
		*activate_level = 30;
		return 27;
	}
	return 0;
}

/* Return the aura mutation the hero's archon subtype develops, or 0 if the hero
 * has none, setting *form_level to the experience level at which it begins forming.
 */
STATIC_OVL int
archon_aura_mutation(int *form_level)
{
	if(!RACE_IF_AASIMAR || flags.aasimar_type != AASIMAR_TYPE_ARCHON)
		return 0;
	*form_level = flags.aasimar_subtype <= LAST_EARLY_AURA_ARCHON_SUBTYPE ? 10 : 21;
	switch(flags.aasimar_subtype){
		case AASIMAR_SUBTYPE_SHIELD_ARCHON:
			return AAT_SHIELD_AURA;
		case AASIMAR_SUBTYPE_TRUMPET_ARCHON:
			return AAT_TRUMPET_AURA;
		case AASIMAR_SUBTYPE_PANAKEIAN_ARCHON:
			return AAT_PANAKEIAN_AURA;
		case AASIMAR_SUBTYPE_JUSTICE_ARCHON:
			return AAT_JUSTICE_AURA;
		case AASIMAR_SUBTYPE_SWORD_ARCHON:
			return AAT_SWORD_AURA;
		case AASIMAR_SUBTYPE_WARDEN_ARCHON:
			return AAT_WARDEN_AURA;
	}
	return 0;
}

/* Return a random mutation from a 0-terminated list, or 0 if the list is null or empty. */
STATIC_OVL int
roll_mutation_list(const int *list)
{
	int count = 0;
	if(!list)
		return 0;
	while(list[count])
		count++;
	return count ? list[rn2(count)] : 0;
}

/* Return a random mutation from a 0-terminated list that the hero neither has
 * nor is forming, or 0 if the list is null or has no such mutation.
 */
STATIC_OVL int
roll_new_mutation(const int *list)
{
	int pool[LAST_MUTATION];
	int count = 0;
	if(!list)
		return 0;
	for(int i = 0; list[i]; i++)
		if(!held_or_forming_mutation(list[i]))
			pool[count++] = list[i];
	return count ? pool[rn2(count)] : 0;
}

/* Advance the aasimar mutation track: activate the forming mutation once its
 * level is reached, then begin forming the first due stage that hasn't been begun.
 * Only one mutation forms at a time.
 */
STATIC_OVL void
check_aasimar_mutations(void)
{
	int chosen = 0;
	int activate_level = 0;
	long stage = 0L;

	if(u.next_aasimar_mutation && u.ulevel >= u.next_aasimar_mutation_level){
		confer_mutation(u.next_aasimar_mutation);
		u.next_aasimar_mutation = 0;
	}
	if(u.next_aasimar_mutation)
		return;

	if(!chosen && !(u.aasimar_mutation_stages & AAS_STAGE_GAZE) && u.ulevel >= 7){
		if(flags.aasimar_type == AASIMAR_TYPE_IKSH_NA)
			chosen = pick_iksh_na_eye_mutation(aasimar_gaze_list());
		else
			chosen = roll_mutation_list(aasimar_gaze_list());
		activate_level = (flags.aasimar_type == AASIMAR_TYPE_DEVA || flags.aasimar_type == AASIMAR_TYPE_SURYA
			|| flags.aasimar_type == AASIMAR_TYPE_PRIMINAL) ? 11 : 10;
		stage = AAS_STAGE_GAZE;
	}
	static const long halo_stages[] = { AAS_STAGE_HALO, AAS_STAGE_HALO_2, AAS_STAGE_HALO_3 };
	for(int i = 0; !chosen && i < SIZE(halo_stages); i++){
		int halo_activate = 0;
		int form_level = aasimar_halo_form_level(halo_stages[i], &halo_activate);
		if(form_level && !(u.aasimar_mutation_stages & halo_stages[i]) && u.ulevel >= form_level){
			chosen = roll_new_mutation(aasimar_halo_list(halo_stages[i]));
			activate_level = halo_activate;
			stage = halo_stages[i];
		}
	}
	static const long grace_stages[] = { AAS_STAGE_GRACE, AAS_STAGE_GRACE_2, AAS_STAGE_GRACE_3 };
	for(int i = 0; !chosen && i < SIZE(grace_stages); i++){
		int grace_activate = 0;
		int form_level = aasimar_grace_form_level(grace_stages[i], &grace_activate);
		if(form_level && !(u.aasimar_mutation_stages & grace_stages[i]) && u.ulevel >= form_level){
			chosen = roll_new_mutation(aasimar_grace_list());
			activate_level = grace_activate;
			stage = grace_stages[i];
		}
	}
	if(!chosen && !(u.aasimar_mutation_stages & AAS_STAGE_ASTRA) && u.ulevel >= 11){
		chosen = roll_mutation_list(aasimar_astra_list());
		activate_level = 15;
		stage = AAS_STAGE_ASTRA;
	}
	if(!chosen && !(u.aasimar_mutation_stages & AAS_STAGE_MUDRA)){
		int mudra_activate = 0;
		int form_level = aasimar_mudra_form_level(&mudra_activate);
		if(form_level && u.ulevel >= form_level){
			chosen = roll_mutation_list(aasimar_mudra_list());
			activate_level = mudra_activate;
			stage = AAS_STAGE_MUDRA;
		}
	}
	if(!chosen && !(u.aasimar_mutation_stages & AAS_STAGE_AURA)){
		int form_level = 0;
		int aura = archon_aura_mutation(&form_level);
		if(aura && u.ulevel >= form_level){
			chosen = aura;
			activate_level = form_level + 3;
			stage = AAS_STAGE_AURA;
		}
	}
	if(!chosen)
		return;

	u.aasimar_mutation_stages |= stage;
	u.next_aasimar_mutation = chosen;
	u.next_aasimar_mutation_level = activate_level;
	announce_forming_mutation(chosen);
}

void
init_natural_mutations()
{
	if(RACE_IF_AASIMAR && flags.aasimar_type == AASIMAR_TYPE_IKSH_NA){
		init_iksh_na_mutations();
		return;
	}
	if(!RACE_IF_TIEFLING) return;
	int mutation_blacklist[] = {TT_TEARS_OF_BLOOD, TT_BLINDING_HAIR, TT_WINGS_1, TT_WINGS_2, TT_WINGS_3, TT_WINGS_4, TT_WINGS_5, TT_WINGS_6, TT_WING_CLAW_1, TT_WING_CLAW_2, TT_MAGIC_BREATHING };
	int possible_mutations[LAST_TIEFLING_TRAIT];
	int possible_count = 0;
	for(int j = 0; mutationtypes[j].mutation; j++){
		int mut = mutationtypes[j].mutation;
		if(mut <= LAST_CULT_MUTATION) continue;
		if(mut > LAST_TIEFLING_TRAIT) continue;
		if(mut == TT_LIGHT && Race_if(PM_DARK_FEY_RI)) continue; // Self-blinding, given darksight
		boolean blacklisted = FALSE;
		for(int k = 0; k < sizeof(mutation_blacklist)/sizeof(int); k++){
			if(mut == mutation_blacklist[k]){
				blacklisted = TRUE;
				break;
			}
		}
		if(blacklisted) continue;
		possible_mutations[possible_count++] = mut;
	}
	int chosen_mutation = possible_mutations[rn2(possible_count)];
	u.next_tiefling_mutation = chosen_mutation;
	u.next_mutation_level = 3;
}

void
check_natural_mutations()
{
	check_aasimar_mutations();
	if(RACE_IF_AASIMAR && flags.aasimar_type == AASIMAR_TYPE_IKSH_NA){
		check_iksh_na_mutations();
		return;
	}
	if(!RACE_IF_TIEFLING) return;
	if(u.next_tiefling_mutation && u.ulevel >= u.next_mutation_level){
		confer_mutation(u.next_tiefling_mutation);
		u.next_tiefling_mutation = 0;
	}
	boolean new_mutation = FALSE;
	for(int i = 0; i < SIZE(flags.tiefling_level); i++){
		if(u.ulevel == flags.tiefling_level[i]){
			new_mutation = TRUE;
			flags.tiefling_level[i] = -1; // Prevent getting the same mutation again
			break;
		}
	}
	if(!new_mutation) return;
	int type_used[MUTATION_TYPE_SIZE];
	boolean fallen_used = FALSE;
	memset(type_used, 0, sizeof(type_used));
	// Mark existing mutation types as used
	for(int j = 0; mutationtypes[j].mutation; j++){
		int i = mutationtypes[j].mutation;
		if(i > LAST_CULT_MUTATION && has_mutation(i) && mutationtypes[j].bodypart >= 0){
			type_used[mutationtypes[j].bodypart] = 1;
		}
		if(is_holy_mut(i) && has_mutation(i)){
			fallen_used = TRUE;
		}
	}
	// Pick a new mutation
	int possible_mutations[LAST_TIEFLING_TRAIT];
	int possible_count = 0;
	for(int j = 0; mutationtypes[j].mutation; j++){
		int mut = mutationtypes[j].mutation;
		if(mut <= LAST_CULT_MUTATION) continue;
		if(mut > LAST_TIEFLING_TRAIT) continue;
		if(mut == TT_LIGHT && Race_if(PM_DARK_FEY_RI)) continue; // Self-blinding, given darksight
		if(mutationtypes[j].bodypart >= 0 && type_used[mutationtypes[j].bodypart]) continue;
		if(has_mutation(mut)) continue;
		if(fallen_used && is_holy_mut(mut)) continue;
		possible_mutations[possible_count++] = mut;
	}
	if(possible_count < 1){
		impossible("No possible tiefling mutations left!");
		return;
	}
	int chosen_mutation = possible_mutations[rn2(possible_count)];
	if(u.next_tiefling_mutation != 0){
		pline("Error recovery: next_tiefling_mutation already set to %d", u.next_tiefling_mutation);
		confer_mutation(u.next_tiefling_mutation);
	}
	u.next_tiefling_mutation = chosen_mutation;
	u.next_mutation_level = u.ulevel + 3;
	if(!Upolyd){
		for(int j = 0; mutationtypes[j].mutation; j++){
			if(chosen_mutation == mutationtypes[j].mutation){
				pline("%s", mutationtypes[j].start_forming);
				break;
			}
		}
	}
}

#define MUT_ABILITY_POISON_CLOUD	1
#define MUT_ABILITY_FIRE_BLAST	2
#define MUT_ABILITY_ICE_BLAST	3
#define MUT_ABILITY_ACID_BLAST	4
#define MUT_ABILITY_SMOKE		5
#define MUT_ABILITY_COLD_CLOUD	6
#define MUT_ABILITY_FROG_CROAK	7
#define MUT_ABILITY_SIREN_SONG	8
int
domutation()
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;
	boolean atleastone = FALSE;
	
	if(u.uen < 20) {
		pline("You don't have enough energy to use a mutation ability.");
		return 0;
	}

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */
	

#define add_ability(letter, string, value) \
	do { \
	Sprintf(buf, (string)); any.a_int = (value); atleastone = TRUE; \
	add_menu(tmpwin, NO_GLYPH, &any, (letter), 0, ATR_NONE, buf, MENU_UNSELECTED); \
	} while (0)

	
	Sprintf(buf, "Abilities");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	if(check_mutation(TT_POISON_CLOUD)) {
		add_ability('P', "Emit stinking cloud", MUT_ABILITY_POISON_CLOUD);
	}
	if(!Upolyd && (has_mutation(TT_FIRE_BLAST_1) || has_mutation(TT_FIRE_BLAST_2))) {
		add_ability('F', "Emit fire blast", MUT_ABILITY_FIRE_BLAST);
	}
	if(!Upolyd && has_mutation(TT_COLD_BLAST)) {
		add_ability('I', "Emit ice blast", MUT_ABILITY_ICE_BLAST);
	}
	if(!Upolyd && has_mutation(TT_ACID_BLAST)) {
		add_ability('A', "Emit acid blast", MUT_ABILITY_ACID_BLAST);
	}
	if(!Upolyd && has_mutation(TT_SMOKE)) {
		add_ability('O', "Emit smoke cloud", MUT_ABILITY_SMOKE);
	}
	if(!Upolyd && has_mutation(TT_COLD_CLOUD)) {
		add_ability('E', "Emit freezing cloud", MUT_ABILITY_COLD_CLOUD);
	}
	if(!Upolyd && has_mutation(TT_FROG_CROAK)) {
		add_ability('C', "Emit a sonic blast", MUT_ABILITY_FROG_CROAK);
	}
	if(!Upolyd && has_mutation(TT_SIREN_SONG)) {
		add_ability('S', "Emit a siren song", MUT_ABILITY_SIREN_SONG);
	}
	if (!atleastone) {
		pline("You don't have any special abilities to use.");
		destroy_nhwindow(tmpwin);
		return 0;
	}
	end_menu(tmpwin, "Choose an ability to use:");
	n = select_menu(tmpwin, PICK_ONE, &selected);
	destroy_nhwindow(tmpwin);
	if (n <= 0) {
		return MOVE_CANCELLED;
	}
	how = selected[0].item.a_int;
	free(selected);
	flags.botl = 1;
	int type = -1;
	int color = -1;
	switch (how) {
		case MUT_ABILITY_FROG_CROAK:{
			struct monst *mtmp;
			for(int i = -1; i < 2; i++){
				for(int j = -1; j < 2; j++){
					if(i == 0 && j == 0) continue;
					if(isok(u.ux+i, u.uy+j)){
						mtmp = m_at(u.ux+i, u.uy+j);
						if(!mtmp) continue;
						if(mtmp->mpeaceful) continue;
						if(nonthreat(mtmp)) continue;
						if(mtmp->mtyp == PM_PALE_NIGHT) continue;
						u.uen -= 20;
						struct attack attk = {AT_HITS, AD_SONC, (u.ulevel+2)/3, 6, .bodypart = ATKBP(MOUTH)};
						xmeleehity(&youmonst, mtmp, &attk, (struct obj **)0, 0, 0, FALSE, 0); //Hits all adjacent targets
						i = 2; //Break outer loop
						break;
					}
				}
			}
		}break;
		case MUT_ABILITY_SIREN_SONG:{
			u.uen -= 20;
			You("sing a hypnotic siren song!");
			if(ACURR(A_CHA) == 25)
				u.bladesong = monstermoves + 8 + 5;
			else
				u.bladesong = monstermoves + (ACURR(A_CHA) - 10)/2 + 5;
			int dx, dy;
			for(struct monst *mtmp = fmon; mtmp; mtmp = mtmp->nmon){
				if(!isok(mtmp->mx, mtmp->my)) continue;
				if(distmin(u.ux, u.uy, mtmp->mx, mtmp->my) > BOLT_LIM) continue;
				if(mtmp->mpeaceful) continue;
				if(nonthreat(mtmp)) continue;
				if(is_deaf(mtmp)) continue;
				if(!resist(mtmp, SPBOOK_CLASS, 0, NOTELL)){
					dx = sgn(u.ux - mtmp->mx);
					dy = sgn(u.uy - mtmp->my);
					if(canseemon(mtmp))
						pline("%s stumbles towards you!", Monnam(mtmp));
					mhurtle(mtmp, dx, dy, 1, TRUE);
				}
			}
		}break;
		case MUT_ABILITY_POISON_CLOUD:
			if(type < 0){
				type = AD_POSN;
				color = EXPL_NOXIOUS;
			}
		case MUT_ABILITY_FIRE_BLAST:
			if(type < 0){
				type = AD_FIRE;
				color = EXPL_FIERY;
			}
		case MUT_ABILITY_SMOKE:
			if(type < 0){
				type = AD_SMOK;
				color = EXPL_FIERY;
			}
		case MUT_ABILITY_ICE_BLAST:
			if(type < 0){
				type = AD_COLD;
				color = EXPL_FROSTY;
			}
		case MUT_ABILITY_COLD_CLOUD:
			if(type < 0){
				type = AD_SLWC;
				color = EXPL_FROSTY;
			}
		case MUT_ABILITY_ACID_BLAST:{
			if(type < 0){
				type = AD_ACID;
				color = EXPL_LIME;
			}
			int x, y;
			if(!throweffect())
				return MOVE_CANCELLED;
			if (distmin(u.ux, u.uy, u.dx, u.dy) > 4) {
				pline("Too far!");
				return MOVE_CANCELLED;
			}
			u.uen -= 20;
			if(type == AD_POSN){
				(void) create_gas_cloud(u.dx, u.dy, 3, (u.ulevel*3+5)/6, TRUE);
			}
			else if(type == AD_SMOK || type == AD_SLWC){
				struct region_arg cloud_data;
				if(type == AD_SMOK)
					cloud_data.damage = 2+u.ulevel;
				else
					cloud_data.damage = 2+(u.ulevel+5)/6;
				cloud_data.adtyp = type;
				(void) create_generic_cloud(u.dx, u.dy, 3, &cloud_data, TRUE);
			}
			else {
				explode(u.dx, u.dy, type, 0, d((u.ulevel + 2)/3, 6), color, 1);
			}
		}
		break;
		default:
			pline("Unknown mutation ability %d.", how);
			return MOVE_CANCELLED;
	}
	return MOVE_STANDARD;
}

STATIC_DCL
void
mutation_effects(int mutation)
{
	/* Vision mutations need the appropriate display components refreshed. */
	if(mutation == TT_INFRAVISION_1){
		see_monsters();
	}
	else if(mutation == TT_EXTRAMISSION_1 || mutation == TT_EXTRAMISSION_2 || mutation == AAT_EXTRAMISSION_1
		|| mutation == AAT_EXTRAMISSION_2
	){
		vision_full_recalc = 1;
	}
	else if(mutation == TT_MAGIC_BREATHING){
		HMagical_breathing |= W_UPGRADE;
	}
	else if(mutation == TT_LIGHT){
		del_light_source((&youmonst)->light);
		new_light_source(LS_MONSTER, (genericptr_t)&youmonst, uemit_light());
	}
	else if(mutation == TT_WEBS){
		HSwimming |= W_UPGRADE;
	}
	else if(mutation == TT_WINGS_1 || mutation == TT_WINGS_2 || mutation == TT_WINGS_3 ||
			mutation == TT_WINGS_4 || mutation == TT_WINGS_5 || mutation == TT_WINGS_6
	){
		HFlying |= W_UPGRADE;
	}
	else if(mutation == AAT_DEVA_SIGHT){
		vision_full_recalc = 1;
		see_monsters();
	}
	else if(mutation == AAT_IKSH_NA_SIGHT){
		see_monsters();
	}
	else if(mutation == AAT_ELADRIN_SIGHT){
		vision_full_recalc = 1;
		see_monsters();
	}
	else if(mutation == AAT_CLOUDFACE_INFRAVISION || mutation == AAT_CLOUDFACE_BLOODSENSE
		|| mutation == AAT_CLOUDFACE_LIFESENSE
	){
		see_monsters();
	}
	else if(mutation == AAT_INVIS_1 || mutation == AAT_INVIS_2 || mutation == AAT_INVIS_3){
		HProtection_from_shape_changers |= W_UPGRADE;
		rescham();
		see_monsters();
	}
	if(is_horn_mut(mutation) && check_mutation(mutation)){
		if(uarmh && !helm_match(&youmonst, uarmh)){
			Your("%s no longer fits!", xname(uarmh));
			if (donning(uarmh)) cancel_don();
			(void) Helmet_off();
		}
	}
}

//Note: amulets are stealable worn items
#define W_UNSTEALABLE (W_AMUL | W_TOOL | W_RING | W_BELT | W_SWAPWEP | W_WEP | W_ARMOR | W_SADDLE)

boolean
has_stealable_item(struct monst *mon)
{
	for(struct obj *obj = mon->minvent; obj; obj = obj->nobj){
		if(obj->owornmask & W_UNSTEALABLE)
			continue;
		return TRUE;
	}
	return FALSE;
}

void
steal_from_monster(struct monst *mon)
{
	int stealable_count = 0;
	struct obj *obj;
	for(obj = mon->minvent; obj; obj = obj->nobj){
		if(obj->owornmask & W_UNSTEALABLE)
			continue;
		stealable_count++;
	}
	if(stealable_count < 1) return;
	stealable_count = rn2(stealable_count);
	for(obj = mon->minvent; obj; obj = obj->nobj){
		if(obj->owornmask & W_UNSTEALABLE)
			continue;
		if(stealable_count > 0){
			stealable_count--;
			continue;
		}
		//Steal this item
		obj_extract_self(obj);
		if(near_capacity() < calc_capacity(obj->owt) || u.uavoid_theft){
		if(canseemon(mon))
			pline("Your tail steals %s from %s and drops it to the %s!", doname(obj), mon_nam(mon), surface(u.ux, u.uy));
			dropy(obj);
		}
		else {
			if(canseemon(mon))
				pline("Your tail steals %s from %s!", doname(obj), mon_nam(mon));
			hold_another_object(obj, "You drop %s!", doname(obj), (const char *)0);
		}
		break;
	}
}

void
mutation_autoattacks()
{
	if(check_mutation(TT_LASHING_TAIL)){
		struct attack attack = {AT_TAIL, AD_PHYS, 1, 2+(u.ulevel/3), .bodypart = ATKBP(TAIL)};
		dogenericattack(&youmonst, &attack, 1, 1);
	}
	if(check_mutation(TT_SNAKE_TAIL)){
		struct attack attack = {AT_OBIT, AD_DRST, 1, 6, .bodypart = ATKBP(OTHER_APPENDAGE)};
		dogenericattack(&youmonst, &attack, 1, 1);
	}
	if(check_mutation(TT_THIEVING_TAIL) || check_mutation(AAT_PRIMINAL_TAIL)){
		int x, y;
		for(int i = -1; i <= 1; i++){
			for(int j = -1; j <= 1; j++){
				if(i == 0 && j == 0) continue;
				x = u.ux + i;
				y = u.uy + j;
				if(!isok(x, y)) continue;
				struct monst *mtmp = m_at(x, y);
				if(!mtmp) continue;
				if(mtmp->mpeaceful) continue;
				if(nonthreat(mtmp)) continue;
				if(!has_stealable_item(mtmp)) continue;
				if(!rn2(8)) continue;
				steal_from_monster(mtmp);
				i = 2; //Break outer loop
				break;
			}
		}
	}
	if(check_mutation(TT_SHADOW_PAIN)){
		struct attack attack = {AT_ESPR, AD_PAIN, 1, 1+(u.ulevel/6), .bodypart = ATKBP(SHADOW)};
		dogenericattack(&youmonst, &attack, 8, 1);
	}
	if(check_mutation(TT_SHADOW_SHRED)){
		struct attack attack = {AT_ESPR, AD_SHRD, 1, 1+(u.ulevel/6), .bodypart = ATKBP(SHADOW)};
		dogenericattack(&youmonst, &attack, 8, 1);
	}
	if(check_mutation(TT_BITING_HAIR)){
		struct attack attack = {AT_OBIT, AD_VAMP, 1, 4, .bodypart = ATKBP(OTHER_APPENDAGE)};
		int mult = (u.ulevel+9)/10;
		dogenericattack(&youmonst, &attack, 8*mult, mult);
	}
	if(check_mutation(TT_SHADOW_CASTER)){
		for(struct monst *mtmp = fmon; mtmp; mtmp = mtmp->nmon){
			if(DEADMONSTER(mtmp)) continue;
			if(distmin(u.ux, u.uy, mtmp->mx, mtmp->my) > BOLT_LIM) continue;
			if(!couldsee(mtmp->mx, mtmp->my)) continue;
			if(mtmp->mpeaceful || nonthreat(mtmp)) continue; //A wizard did it
			if(rn2(10) != 0) continue; //10% chance to cast
			int spell;
			switch(rn2(5)){
				case 0: spell = PARALYZE; break;
				case 1: spell = DESTRY_WEPN; break;
				case 2: spell = DESTRY_ARMR; break;
				case 3: spell = OPEN_WOUNDS; break;
				default: spell = PSI_BOLT; break;
			}
			struct attack attack = {AT_MAGC, AD_SPEL, 0, 6, .bodypart = ATKBP(SHADOW)};
			cast_spell(&youmonst, mtmp, &attack, spell, mtmp->mx, mtmp->my);
			break;
		}
		
	}
}

/* Return TRUE if the light of the hero's halo falls on mon (which may be the hero). */
boolean
in_halo_light(struct monst *mon)
{
	if(u.uswallow)
		return (mon == &youmonst || mon == u.ustuck) && !uswallow_indark();
	if(mon == &youmonst)
		return dimness(u.ux, u.uy) <= 0;
	return couldsee(mon->mx, mon->my)
		&& in_hero_light_circle(mon->mx, mon->my, uemit_light())
		&& dimness(mon->mx, mon->my) <= 0;
}

/* Return TRUE if the hero's shield aura makes an attack on mdef miss. */
boolean
shield_aura_deflects(struct monst *mdef)
{
	return mdef != &youmonst && uarms && check_mutation(AAT_SHIELD_AURA)
		&& mdef->mtame && in_beast_mastery_range(mdef) && !rn2(3);
}

/* Return TRUE if the hero's grace mutation empowers mon (hero at or above half HP version). */
boolean
grace_empowers(struct monst *mon, int grace)
{
	if(!check_grace(grace) || !mon)
		return FALSE;
	return mon == &youmonst || (mon->mtame && in_beast_mastery_range(mon));
}

/* Return TRUE if the hero's grace mutation empowers mon (hero at full HP version). */
boolean
full_grace_empowers(struct monst *mon, int grace)
{
	if(!check_full_grace(grace) || !mon)
		return FALSE;
	return mon == &youmonst || (mon->mtame && in_beast_mastery_range(mon));
}

/* Return TRUE if the hero's grace mutation empowers mon (hero at or above 90% of max HP version). */
boolean
high_grace_empowers(struct monst *mon, int grace)
{
	if(!check_high_grace(grace) || !mon)
		return FALSE;
	return mon == &youmonst || (mon->mtame && in_beast_mastery_range(mon));
}

/* Return TRUE if the hero's dodge grace makes an attack on mdef miss. */
boolean
grace_dodges(struct monst *mdef)
{
	if(full_grace_empowers(mdef, AAT_GRACE_DODGE))
		return !rn2(3);
	if(grace_empowers(mdef, AAT_GRACE_DODGE))
		return !rn2(6);
	return FALSE;
}

/* Return TRUE if the hero's gifts grace charms magr, a monster about to attack
 * mdef in melee, into dropping its wielded weapons at mdef's feet instead.
 */
boolean
grace_gifts_offer(struct monst *magr, struct monst *mdef)
{
	struct obj *offered[2];
	struct obj *otmp;
	boolean youdef = (mdef == &youmonst);
	int tx = youdef ? u.ux : mdef->mx;
	int ty = youdef ? u.uy : mdef->my;
	char buf[BUFSZ];
	int n = 0;
	int i;

	if(magr == &youmonst || !full_grace_empowers(mdef, AAT_GRACE_GIFTS) || !intelligent_mon(magr))
		return FALSE;
	if(!could_seduce(mdef, magr, (struct attack *)0))
		return FALSE;
	if(!youdef && !intelligent_mon(mdef))
		return FALSE;
	if((otmp = MON_WEP(magr)) && !(otmp->cursed && !is_weldproof_mon(magr)))
		offered[n++] = otmp;
	if((otmp = MON_SWEP(magr)) && otmp != MON_WEP(magr) && !(otmp->cursed && !is_weldproof_mon(magr)))
		offered[n++] = otmp;
	if(!n)
		return FALSE;
	if(youdef ? resist(magr, '\0', 0, NOTELL) : mm_resist(magr, mdef, 0, NOTELL))
		return FALSE;

	if(youdef || canseemon(magr) || canseemon(mdef)){
		Sprintf(buf, "%s %s", mhis(magr), xname(offered[0]));
		if(n > 1)
			Sprintf(eos(buf), " and %s %s", mhis(magr), xname(offered[1]));
		pline("%s looks %s by %s grace and tries to offer %s %s.",
			Monnam(magr), 
			could_seduce(mdef, magr, (struct attack *)0) == 1 ? "smitten" : "charmed",
			youdef ? "your" : s_suffix(mon_nam(mdef)),
			youdef ? "you" : mhim(mdef),
			buf);
	}
	for(i = 0; i < n; i++){
		otmp = offered[i];
		magr->misc_worn_check &= ~otmp->owornmask;
		setmnotwielded(magr, otmp);
		otmp->owornmask = 0L;
		obj_extract_self(otmp);
		if(!flooreffects(otmp, tx, ty, "fall")){
			place_object(otmp, tx, ty);
			stackobj(otmp);
		}
	}
	magr->weapon_check = NEED_WEAPON;
	newsym(tx, ty);
	return TRUE;
}

/* Return TRUE if otmp is crystal plate mail lightened by the hero's crystal grace (full: the full-HP version). */
boolean
crystal_grace_armor(struct obj *otmp, boolean full)
{
	struct monst *wearer;

	if(otmp->otyp != CRYSTAL_PLATE_MAIL)
		return FALSE;
	wearer = (otmp->where == OBJ_INVENT) ? &youmonst
		: (otmp->where == OBJ_MINVENT) ? otmp->ocarry : (struct monst *)0;
	return full ? full_grace_empowers(wearer, AAT_GRACE_CRYSTAL) : grace_empowers(wearer, AAT_GRACE_CRYSTAL);
}

/* Return TRUE if otmp is crystal armor whose enchantment the hero's crystal grace strengthens. */
boolean
crystal_grace_enchants(struct obj *otmp)
{
	struct monst *wearer;

	if(!is_crystal_armor(otmp))
		return FALSE;
	wearer = (otmp->where == OBJ_INVENT) ? &youmonst
		: (otmp->where == OBJ_MINVENT) ? otmp->ocarry : (struct monst *)0;
	return grace_empowers(wearer, AAT_GRACE_CRYSTAL);
}

/* Return the damage type of the mantle granted by the hero's mantle grace, determined by the hero's eladrin subtype. */
int
grace_mantle_adtyp(void)
{
	static boolean warned = FALSE;

	switch(flags.aasimar_subtype){
		case AASIMAR_SUBTYPE_COURE:
		case AASIMAR_SUBTYPE_TULANI:
			return AD_STAR;
		case AASIMAR_SUBTYPE_NOVIERE:
		case AASIMAR_SUBTYPE_UISCERRE:
			return AD_WET;
		case AASIMAR_SUBTYPE_FIRRE:
			return AD_FIRE;
		case AASIMAR_SUBTYPE_SHIERE:
			return AD_COLD;
		case AASIMAR_SUBTYPE_CAILLEA:
			return AD_HOLY;
		case AASIMAR_SUBTYPE_GHAELE:
			return AD_ELEC;
		case AASIMAR_SUBTYPE_BRALANI:
			return AD_BLND;
		case AASIMAR_SUBTYPE_BRIGHID:
			return AD_PYCL;
		case AASIMAR_SUBTYPE_GAE:
			return AD_SESN;
	}
	if(wizard && !warned){
		pline("Warning: no mantle for aasimar subtype %d; using a mantle of silver stars.", flags.aasimar_subtype);
		warned = TRUE;
	}
	return AD_STAR;
}

/* Return TRUE if the hero's astra mutation empowers mon (the hero's pets only). */
boolean
astra_empowers(struct monst *mon, int astra)
{
	return check_mutation(astra) && mon && mon != &youmonst && mon->mtame
		&& in_beast_mastery_range(mon);
}

/* Return TRUE if (x,y) is within the circle of the given radius centered on
 * (cx,cy) that vision uses for light sources and x-ray vision (ignoring line of
 * sight). A radius below 1 covers only the center.
 */
boolean
within_vision_circle(int cx, int cy, int x, int y, int radius)
{
	int dx = abs(x - cx);
	int dy = abs(y - cy);

	if(radius < 1)
		return !dx && !dy;
	if(radius > MAX_RADIUS)
		radius = MAX_RADIUS;
	return dy <= radius && dx <= circle_data[circle_start[radius] + dy];
}

/* Return TRUE if (x,y) is within the circle a light source of the given radius
 * centered on the hero would light (ignoring line of sight).
 */
STATIC_OVL boolean
in_hero_light_circle(int x, int y, int radius)
{
	return within_vision_circle(u.ux, u.uy, x, y, radius);
}

/* Apply the sunlight of the hero's dawn halo to mtmp. Return TRUE if mtmp was killed or petrified. */
STATIC_OVL boolean
halo_sunlight(struct monst *mtmp)
{
	if(mtmp->mtyp == PM_GREMLIN || mtmp->mtyp == PM_HUNTING_HORROR){
		if(canseemon(mtmp))
			pline("The light of dawn sears %s!", mon_nam(mtmp));
		return m_losehp(mtmp, mtmp->mhp, TRUE, "dawn light");
	}
	if(mtmp->data->mlet == S_TROLL && !resists_ston(mtmp) && !munstone(mtmp, TRUE)){
		if(canseemon(mtmp))
			pline("The light of dawn strikes %s!", mon_nam(mtmp));
		minstapetrify(mtmp, TRUE, FALSE);
		return TRUE;
	}
	return FALSE;
}

void
mutation_auras(void)
{
	// (has_mutation(TT_TEARS_OF_BLOOD) \
	//  || has_mutation(TT_FLAMING_HAIR) \
	//  || has_mutation(TT_FROSTY_HAIR) \
	// )
	if(check_mutation(TT_TEARS_OF_BLOOD) && !u.uswallow){
		int dmg = (u.ulevel+9)/10;
		for(struct monst *mtmp = fmon; mtmp; mtmp = mtmp->nmon){
			if(DEADMONSTER(mtmp)) continue;
			if(!mon_can_see_you(mtmp)) continue;
			if(distmin(u.ux, u.uy, mtmp->mx, mtmp->my) > BOLT_LIM) continue;
			if(!has_blood_mon(mtmp) || eyecount(mtmp->data) < 1) continue;
			if(mtmp->mtyp == PM_FLOATING_EYE)
				mtmp->mhp = max(mtmp->mhp - dmg*2, 1);
			else
				mtmp->mhp = max(mtmp->mhp - dmg, 1);
		}
	}
	if(check_mutation(TT_FLAMING_HAIR)){
		int dmg = d(u.ulevel/10+1, 4);
		
		for(struct monst *mtmp = fmon; mtmp; mtmp = mtmp->nmon){
			if(DEADMONSTER(mtmp)) continue;
			if(!couldsee(mtmp->mx, mtmp->my)) continue;
			if(mtmp->mpeaceful || nonthreat(mtmp)) continue; //A wizard did it
			if(resists_fire(mtmp)) continue;
			if(dmg - distu(mtmp->mx, mtmp->my) < 1) continue;
			if(u.uswallow){
				if(u.ustuck == mtmp)
					m_losehp(mtmp, 8*dmg, TRUE, "fiery hair");
			}
			else m_losehp(mtmp, dmg - distu(mtmp->mx, mtmp->my), TRUE, "fiery aura");
		}
	}
	if(check_mutation(TT_FROSTY_HAIR)){
		int dmg = d(u.ulevel/10+1, 4);
		
		for(struct monst *mtmp = fmon; mtmp; mtmp = mtmp->nmon){
			if(DEADMONSTER(mtmp)) continue;
			if(!couldsee(mtmp->mx, mtmp->my)) continue;
			if(mtmp->mpeaceful || nonthreat(mtmp)) continue; //A wizard did it
			if(resists_cold(mtmp)) continue;
			if(dmg - distu(mtmp->mx, mtmp->my) < 1) continue;
			if(u.uswallow){
				if(u.ustuck == mtmp)
					m_losehp(mtmp, 8*dmg, TRUE, "frosty aura");
			}
			else m_losehp(mtmp, dmg - distu(mtmp->mx, mtmp->my), TRUE, "frosty aura");
		}
	}
	if(check_mutation(AAT_WARDEN_AURA)){
		for(struct monst *mtmp = fmon; mtmp; mtmp = mtmp->nmon){
			if(DEADMONSTER(mtmp)) continue;
			if(!mtmp->mtame || !in_beast_mastery_range(mtmp)) continue;
			mtmp->mhp = min(mtmp->mhp + 7, mtmp->mhpmax);
		}
	}
	if(THREATENING_MUDRA)
		cast_abjuration(TRUE);
	if(check_mutation(AAT_PANAKEIAN_AURA)){
		for(struct monst *mtmp = fmon; mtmp; mtmp = mtmp->nmon){
			if(DEADMONSTER(mtmp)) continue;
			if(!mtmp->mtame || !in_beast_mastery_range(mtmp)) continue;
			mon_fix_troubles(mtmp, 0);
		}
	}
	if(AASIMAR_HALOS){
		int dmg = u.ulevel < 10 ? d(1,3) : d(u.ulevel/10, 7);
		int silver_dmg = d(1, u.ulevel*2/3+1);
		int heal = u.ulevel/4+1;
		int radius = uemit_light();
		int sun_radius = radius/4;

		boolean healing = check_mutation(AAT_HALO_DAWN_HEAL) || check_mutation(AAT_FACE_HEAL);

		if(healing && in_halo_light(&youmonst) && u.uhp < u.uhpmax){
			u.uhp = min(u.uhp + heal, u.uhpmax);
			flags.botl = 1;
		}
		for(struct monst *mtmp = fmon; mtmp; mtmp = mtmp->nmon){
			if(DEADMONSTER(mtmp)) continue;
			if(!in_halo_light(mtmp)) continue;
			if(mtmp->mtame){
				if(healing)
					mtmp->mhp = min(mtmp->mhp + heal, mtmp->mhpmax);
				continue;
			}
			if(mtmp->mpeaceful || nonthreat(mtmp)) continue; //A wizard did it
			if(AASIMAR_DAWN_HALO && in_hero_light_circle(mtmp->mx, mtmp->my, sun_radius) && halo_sunlight(mtmp)) continue;
			/* engulfed: distmin() is 0, which mustn't add damage */
			int falloff = max(distmin(u.ux, u.uy, mtmp->mx, mtmp->my) - 1, 0);
			int total = 0;
			if((check_mutation(AAT_HALO_PURIFY) || check_mutation(AAT_HALO_DAWN_PURIFY) || check_mutation(AAT_FACE_PURIFY))
				&& opposes_u_holiness(mtmp)
			)
				total += max(dmg - falloff, 0);
			if((check_mutation(AAT_HALO_CLEANSE) || check_mutation(AAT_HALO_DAWN_CLEANSE) || check_mutation(AAT_FACE_CLEANSE))
				&& is_undead(mtmp->data)
			){
				total += max(dmg - falloff, 0);
				if(mtmp->encouraged > -7)
					mtmp->encouraged--;
			}
			if(check_mutation(AAT_HALO_ORDER) && is_chaotic_mon(mtmp))
				total += max(dmg - falloff, 0);
			if(check_mutation(AAT_FACE_FREEDOM) && is_lawful_mon(mtmp))
				total += max(dmg - falloff, 0);
			if((check_mutation(AAT_HALO_DAWN_FIRE) || check_mutation(AAT_FACE_FIRE)) && !resists_fire(mtmp))
				total += max(dmg - falloff, 0);
			if(check_mutation(AAT_HALO_SILVER) && hates_silver(mtmp->data))
				total += max(silver_dmg - falloff, 0);
			if(total > 0)
				m_losehp(mtmp, u.uswallow ? 8*total : total, TRUE, "halo");
		}
	}
}

int
uemit_light()
{
	int radius = check_mutation(TT_LIGHT) ? 2 : 0;
	radius = max(radius, emits_light(youracedata));
	if(u.ulevel >= 7){
		if(flags.aasimar_type == AASIMAR_TYPE_ARCHON){
			radius = max(radius, u.ulevel/7);
		}
		else if(flags.aasimar_type == AASIMAR_TYPE_SERAPH
			|| flags.aasimar_type == AASIMAR_TYPE_SURYA){
			radius = max(radius, u.ulevel/4);
			if(flags.aasimar_type == AASIMAR_TYPE_SURYA && u.ulevel >= 14)
				radius++;
		}
		else if(flags.aasimar_type == AASIMAR_TYPE_DEVA
			|| flags.aasimar_type == AASIMAR_TYPE_PRIMINAL){
			radius = 1;
		}
		else if(flags.aasimar_type == AASIMAR_TYPE_CLOUDFACE){
			if(u.ulevel >= 21){
				radius = 1 + (u.ulevel - 21)/4;
			}
		}
	}
	return radius;
}

void
recover_seraph_eye()
{
	u.seraph_eyes++;
	if(u.seraph_eyes == SE_SUN){
		pline("You have recovered the first of your stolen eyes, regaining the ability to see the light of sun and moon.");
		vision_full_recalc = 1;
	}
	else if(u.seraph_eyes == SE_INVIS){
		pline("You have regained the ability to see invisible creatures.");
		see_monsters();
	}
	else if(u.seraph_eyes == SE_HIDDEN){
		pline("You have regained the ability to see hidden things.");
		findit();
	}
	else if(u.seraph_eyes == SE_CURSES){
		pline("You have regained the ability to see evil wherever it hides.");
	}
	else if(u.seraph_eyes == SE_MAGIC){
		pline("You have regained the ability to see magical auras.");
	}
	else if(u.seraph_eyes == SE_ALL){
		pline("You have regained the ability to see in all conditions.");
		vision_full_recalc = 1;
	}
	else if(u.seraph_eyes == SE_FUTURE){
		pline("You have reclaimed your final eye, allowing you to see an instant into the future.");
	}
}

/* Turn the hero's held or forming deva mudras into their priminal (pseudopod) forms. Returns TRUE if any mudra was changed. */
boolean
priminal_mudras(void)
{
	static const int mudras[][2] = {
		{ AAT_FEARLESSNESS_MUDRA, AAT_PRIMINAL_FEARLESSNESS_MUDRA },
		{ AAT_MEDITATION_MUDRA, AAT_PRIMINAL_MEDITATION_MUDRA },
		{ AAT_CLARITY_MUDRA, AAT_PRIMINAL_CLARITY_MUDRA },
		{ AAT_THREATENING_MUDRA, AAT_PRIMINAL_THREATENING_MUDRA },
	};
	boolean found = FALSE;

	for(int i = 0; i < SIZE(mudras); i++){
		if(has_mutation(mudras[i][0])){
			remove_mutation(mudras[i][0]);
			add_mutation(mudras[i][1]);
			found = TRUE;
		}
		if(u.next_aasimar_mutation == mudras[i][0]){
			u.next_aasimar_mutation = mudras[i][1];
			found = TRUE;
		}
	}
	return found;
}

STATIC_DCL void
mumbling_mouths_detect()
{
	int x, y;
	struct obj *obj, *otmp;
	struct monst *mtmp;

	/* Identify appearance of carried items */
	for (obj = invent; obj; obj = obj->nobj)
		do_dknown_of(obj);

	/* Clear remembered object glyphs that no longer reflect reality */
	clear_stale_map(ALL_CLASSES, 0, TRUE);

	/* Buried objects */
	if (!Is_paradise(&u.uz)) {
		for (obj = level.buriedobjlist; obj; obj = obj->nobj) {
			do_dknown_of(obj);
			map_object(obj, 1);
		}
	}

	/* Floor objects: show only the top of each pile */
	for (x = 1; x < COLNO; x++)
		for (y = 0; y < ROWNO; y++)
			for (obj = level.objects[x][y]; obj; obj = obj->nexthere) {
				do_dknown_of(obj);
				if (obj == level.objects[x][y])
					map_object(obj, 1);
			}

	/* Objects in monster inventories: show at the monster's location */
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if (DEADMONSTER(mtmp)) continue;
		for (obj = mtmp->minvent; obj; obj = obj->nobj) {
			do_dknown_of(obj);
			if (obj == mtmp->minvent) {
				otmp = obj;
				otmp->ox = mtmp->mx;
				otmp->oy = mtmp->my;
				map_object(otmp, 1);
			}
		}
	}

	newsym(u.ux, u.uy);
}

STATIC_DCL void
mumbling_mouths_turn()
{
	struct monst *mtmp, *mtmp2;
	int range, once;

	range = BOLT_LIM + (u.ulevel / 5);
	range *= range;
	once = 0;

	for (mtmp = fmon; mtmp; mtmp = mtmp2) {
		mtmp2 = mtmp->nmon;

		if (DEADMONSTER(mtmp)) continue;
		if (!couldsee(mtmp->mx, mtmp->my) ||
			distu(mtmp->mx, mtmp->my) > range
		) continue;

		if (!mtmp->mpeaceful &&
			(is_undead(mtmp->data) ||
			 (is_demon(mtmp->data) && (u.ulevel > (MAXULEV/2))))
		) {
			mtmp->msleeping = 0;
			if (!Confusion && !resist(mtmp, '\0', 0, TELL)) {
				// if (!once++) {
				// 	Your("mouths whisper a ward against the unholy.");
				// }
				monflee(mtmp, 0, FALSE, TRUE);
			}
		}
	}
}

void
mumbling_mouths()
{
	int chance = 3000;
	if(Insight < 7) chance *= 3;
	else if(Insight < 14) chance *= 2.333;
	else if(Insight < 21) chance *= 1.667;
	if(u_breath_penalty()) chance *= 3*u_breath_penalty();
	chance = chance * pow(0.9, u.ulevel-1);
	if(rn2(chance)) return;
	if(Insight < 21)
		You("suddenly whisper an involuntary prayer!");
	else
		Your("mumbling mouths whisper a prayer with startling clarity!");
	switch(rn2(20)){
		//divination effects
		case 0:
			if(!level.flags.nommap){
				do_mapping();
				break;
			}
		case 1:
			do_vicinity_map(u.ux,u.uy);
			break;
		case 2:
			mumbling_mouths_detect();
			break;
		//healing effects
		case 3:{
			struct obj *pseudo;
			pseudo = mksobj(SPE_MASS_HEALING, MKOBJ_NOINIT);
			pseudo->blessed = pseudo->cursed = 0;
			pseudo->quan = 20L;			/* do not let useup get it */
			cast_mass_healing(pseudo);
			obfree(pseudo, (struct obj *)0);
			break;
		}
		case 4:
			healing_zap(&youmonst, SPE_EXTRA_HEALING, SPBOOK_CLASS, (boolean *)0, (boolean *)0, FALSE);
			//And pets
			for(struct monst *mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
				if(DEADMONSTER(mtmp)) continue;
				if(!mtmp->mtame) continue;
				healing_zap(mtmp, SPE_EXTRA_HEALING, SPBOOK_CLASS, (boolean *)0, (boolean *)0, FALSE);
			}
			break;
		case 5:
			healing_zap(&youmonst, SPE_FULL_HEALING, SPBOOK_CLASS, (boolean *)0, (boolean *)0, FALSE);
			//And pets
			for(struct monst *mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
				if(DEADMONSTER(mtmp)) continue;
				if(!mtmp->mtame) continue;
				healing_zap(mtmp, SPE_FULL_HEALING, SPBOOK_CLASS, (boolean *)0, (boolean *)0, FALSE);
			}
			break;
		//clerical effects
		case 6:
			mumbling_mouths_turn();
			break;
		case 7:{
			cast_protection();
			//And pets
			int prot = -1 * u.ulevel;
			for(struct monst *mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
				if(DEADMONSTER(mtmp)) continue;
				if(!mtmp->mtame) continue;
				mtmp->mstdy = max(prot, mtmp->mstdy + prot);
			}
		}break;
		case 8:
			cast_abjuration(TRUE);
			break;
		//prayer effects
		case 9:{
			int trouble = in_trouble();
			if(trouble > 0) fix_worst_trouble(trouble);
			break;
		}
		case 10:{
			int trouble = in_trouble();
			if(trouble != 0) fix_worst_trouble(trouble);
			break;
		}
		case 11:
			golden_glow();
			break;
		case 12:
			prayer_benefit_intrinsic(0, TRUE);
			//And pets
			for(struct monst *mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
				if(DEADMONSTER(mtmp)) continue;
				if(!mtmp->mtame) continue;
				random_monster_resistance(mtmp);
			}
			break;
		//sac effects
		case 13:
			god_benefit_boost_ability();
			break;
		case 14:
			if(!god_benefit_enchant_item(&youmonst)){
				//Try pets
				for(struct monst *mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
					if(DEADMONSTER(mtmp)) continue;
					if(!mtmp->mtame) continue;
					god_benefit_enchant_item(mtmp);
				}
			}
			break;
		case 15:
			god_benefit_identify_item();
			break;
		case 16:
			god_benefit_give_intrinsic();
			//And pets
			for(struct monst *mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
				if(DEADMONSTER(mtmp)) continue;
				if(!mtmp->mtame) continue;
				random_monster_resistance(mtmp);
			}
			break;
		case 17:
			god_benefit_repair_item();
			//And pets
			for(struct monst *mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
				if(DEADMONSTER(mtmp)) continue;
				if(!mtmp->mtame) continue;
				mrepair_item(mtmp);
			}
			break;
		case 18:
			god_benefit_fix_buc();
			//And pets
			for(struct monst *mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
				if(DEADMONSTER(mtmp)) continue;
				if(!mtmp->mtame) continue;
				mfix_buc(mtmp);
			}
			break;
		//luck effect
		case 19:
			if (u.uluck < 0) u.uluck = 0;
			change_luck(Insight >= 21 ? 7 : 3);
			break;
	}
}