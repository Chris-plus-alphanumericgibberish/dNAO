#include "hack.h"
#include "mutations.h"

/* mutation lists */
const int shubbie_mutation_list[] = {ABHORRENT_SPORE,
						 CRAWLING_FLESH,
						 SHUB_RADIANCE,
						 TENDRIL_HAIR,
						 SHIFTING_MIND,
						 SHUB_CLAWS,
						 MIND_STEALER,
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
	{ TENDRIL_HAIR, -1, "hairlike tendrils", "Your body is covered with hairlike tentrils." },
	{ SHIFTING_MIND, -1, "shifting mind", "Your mind has shifted."},
	{ SHUB_CLAWS, HAND, "pointed claws", "Your fingernails can fuse into your fingers and transform into long claws."},
	{ MIND_STEALER, TONGUE, "mind-stealing tongue", "Your long thin tongue can slip into others' thoughts."},
	{ YOG_GAZE_1, EYE, "eyes of Yog-Sothoth", "Your eyes burn with magenta fire."},
	{ YOG_GAZE_2, EYE, "frenzy of Yog-Sothoth", "Your eyes blaze with magenta fire."},
	{ TWIN_MIND, -1, "chanting tentacles", "Your waist-tentacles chant strange spells."},
	{ TWIN_DREAMS, -1, "mind blasts", "You emit deadly dreams."},
	// { BY_THE_SMELL, -1, "bladder of Yog-Sothoth", "A gas-filled bladder swells then vanishes. An unholy stench fills the air!"},
	{ TWIN_SAVE, -1, "empathic link", "You feel a strong connection to your twin."},
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
	for (i = 0; mutationtypes[i].mutation; i++){
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
	return FALSE;
}
