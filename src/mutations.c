#include <math.h>
#include "hack.h"
#include "mattkbp.h"
#include "mutations.h"

STATIC_DCL void mutation_effects(int);

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
	{ TT_FIRE_BLAST_1, SMELL_TRAIT, "fiery blasts", "You can emit fiery blasts.", "sulferous smell", "You stink of sulfur."},
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
	return FALSE;
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
			TT_BEHOLDER, TT_DISCOVERY_1, TT_MANY_ODD_EYES
		};
		chosen = ROLL_FROM(extra_eye);
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
		/* identical-effect groups: if any member is held, exclude all members */
		static const int odd_eyes_grp[] = {
			TT_ODD_EYES_1, TT_ODD_EYES_2, TT_ODD_EYES_3, TT_MANY_ODD_EYES
		};
		static const int extramission_grp[] = { TT_EXTRAMISSION_1, TT_EXTRAMISSION_2 };
		static const int discovery_grp[]    = { TT_DISCOVERY_1, TT_DISCOVERY_2 };
		boolean has_odd_eyes = FALSE, has_extramission = FALSE, has_discovery = FALSE;
		for(int k = 0; k < SIZE(odd_eyes_grp); k++)
			if(has_mutation(odd_eyes_grp[k])){ has_odd_eyes = TRUE; break; }
		for(int k = 0; k < SIZE(extramission_grp); k++)
			if(has_mutation(extramission_grp[k])){ has_extramission = TRUE; break; }
		for(int k = 0; k < SIZE(discovery_grp); k++)
			if(has_mutation(discovery_grp[k])){ has_discovery = TRUE; break; }
		int pool[32]; int count = 0;
		for(int j = 0; mutationtypes[j].mutation; j++){
			int mut = mutationtypes[j].mutation;
			if(mutationtypes[j].bodypart != EYE_BP) continue;
			if(mut <= LAST_CULT_MUTATION) continue;
			if(has_mutation(mut)) continue;
			boolean skip = FALSE;
			for(int k = 0; k < SIZE(exclude); k++)
				if(mut == exclude[k]){ skip = TRUE; break; }
			if(skip) continue;
			if(has_odd_eyes){
				boolean in_grp = FALSE;
				for(int k = 0; k < SIZE(odd_eyes_grp); k++)
					if(mut == odd_eyes_grp[k]){ in_grp = TRUE; break; }
				if(in_grp) continue;
			}
			if(has_extramission){
				boolean in_grp = FALSE;
				for(int k = 0; k < SIZE(extramission_grp); k++)
					if(mut == extramission_grp[k]){ in_grp = TRUE; break; }
				if(in_grp) continue;
			}
			if(has_discovery){
				boolean in_grp = FALSE;
				for(int k = 0; k < SIZE(discovery_grp); k++)
					if(mut == discovery_grp[k]){ in_grp = TRUE; break; }
				if(in_grp) continue;
			}
			pool[count++] = mut;
		}
		if(count > 0){
			chosen = pool[rn2(count)];
			u.next_mutation_level = 21;
		}
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

void
init_natural_mutations()
{
	if(Race_if(PM_DOKKIMAR)
		|| (Race_if(PM_AASIMAR) && flags.aasimar_type == AASIMAR_TYPE_IKSH_NA)){
		init_iksh_na_mutations();
		return;
	}
	if(!Race_if(PM_TIEFLING) && !Race_if(PM_DARK_FEY_RI)) return;
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
	if(Race_if(PM_DOKKIMAR)
		|| (Race_if(PM_AASIMAR) && flags.aasimar_type == AASIMAR_TYPE_IKSH_NA)){
		check_iksh_na_mutations();
		return;
	}
	if(!Race_if(PM_TIEFLING) && !Race_if(PM_DARK_FEY_RI)) return;
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
	if(mutation == TT_INFRAVISION_1){
		HInfravision |= W_UPGRADE;
		doredraw();
	}
	else if(mutation == TT_EXTRAMISSION_1 || mutation == TT_EXTRAMISSION_2){
		HExtramission |= W_UPGRADE;
		doredraw();
	}
	else if(mutation == TT_ECHOLOCATION){
		HEcholocation |= W_UPGRADE;
		doredraw();
	}
	else if(mutation == TT_MAGIC_BREATHING){
		HMagical_breathing |= W_UPGRADE;
	}
	else if(mutation == TT_LIGHT){
		del_light_source((&youmonst)->light);
		new_light_source(LS_MONSTER, (genericptr_t)&youmonst, uemit_light());
		HLowlightsight |= W_UPGRADE;
		doredraw();
	}
	else if(mutation == TT_WEBS){
		HSwimming |= W_UPGRADE;
	}
	else if(mutation == TT_WINGS_1 || mutation == TT_WINGS_2 || mutation == TT_WINGS_3 ||
			mutation == TT_WINGS_4 || mutation == TT_WINGS_5 || mutation == TT_WINGS_6
	){
		HFlying |= W_UPGRADE;
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

void
mutation_auras()
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
		else if(flags.aasimar_type == AASIMAR_TYPE_SERAPH){
			radius = max(radius, u.ulevel/4);
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
		doredraw();
	}
	else if(u.seraph_eyes == SE_INVIS){
		pline("You have regained the ability to see invisible creatures.");
		HSee_invisible |= W_UPGRADE;
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
		HExtramission |= W_UPGRADE;
		doredraw();
	}
	else if(u.seraph_eyes == SE_FUTURE){
		pline("You have reclaimed your final eye, allowing you to see an instant into the future.");
	}
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
			cast_abjuration();
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