/*	SCCS Id: @(#)mondata.h	3.4	2003/01/08	*/
/* Copyright (c) 1989 Mike Threepoint				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef MONDATA_H
#define MONDATA_H

#define verysmall(ptr)		((ptr)->msize < MZ_SMALL)
#define bigmonst(ptr)		((ptr)->msize >= MZ_LARGE)

#define MON_BAB(mon)		((mon)->data->mflagsf&MF_BAB_FULL ? 1.0 : (mon)->data->mflagsf&MF_BAB_HALF ? .5 : .75)

#define lev_limit_30(ptr)		((ptr)->mflagsf&MF_LEVEL_30)
#define lev_limit_45(ptr)		((ptr)->mflagsf&MF_LEVEL_45)

#define has_phys_scaling(ptr)		((ptr)->mflagsf&MF_PHYS_SCALING)

#define pm_resistance(ptr,typ)	(((ptr)->mresists & (typ)) != 0)
#define mon_intrinsic(mon,typ)	(((mon)->mintrinsics[((typ)-1)/32] & (0x1L << ((typ)-1)%32)) != 0)
#define mon_extrinsic(mon,typ)	(((mon)->mextrinsics[((typ)-1)/32] & (0x1L << ((typ)-1)%32)) != 0)
#define mon_acquired_trinsic(mon,typ) (((mon)->acquired_trinsics[((typ)-1)/32] & (0x1L << ((typ)-1)%32)) != 0)
#define mon_resistance(mon,typ)	(mon_intrinsic(mon,typ) || mon_extrinsic(mon,typ) || (typ == SWIMMING && Is_waterlevel(&u.uz)) || (typ == TELEPORT && mad_monster_turn(mon, MAD_NON_EUCLID) && !(mon)->mpeaceful) || (typ == TELEPORT_CONTROL && mad_monster_turn(mon, MAD_NON_EUCLID)))

#define species_resists_fire(mon)	(((mon)->data->mresists & MR_FIRE) != 0)
#define species_resists_cold(mon)	(((mon)->data->mresists & MR_COLD) != 0)
#define species_resists_sleep(mon)	(((mon)->data->mresists & MR_SLEEP) != 0)
#define species_resists_disint(mon)	(((mon)->data->mresists & MR_DISINT) != 0)
#define species_resists_elec(mon)	(((mon)->data->mresists & MR_ELEC) != 0)
#define species_resists_poison(mon)	(((mon)->data->mresists & MR_POISON) != 0)
#define species_resists_acid(mon)	(((mon)->data->mresists & MR_ACID) != 0)
#define species_resists_ston(mon)	(((mon)->data->mresists & MR_STONE) != 0)
#define species_resists_drain(mon)	(((mon)->data->mresists & MR_DRAIN) != 0)
#define species_resists_sickness(mon)	(((mon)->data->mresists & MR_SICK) != 0)
#define species_resists_magic(mon)	(((mon)->data->mresists & MR_MAGIC) != 0)
#define species_reflects(mon)		(((mon)->data->mresists & MR_REFLECT) != 0)

#define	resist_attacks(ptr)	((((ptr)->mflagsg & MG_WRESIST) != 0L))
#define	resist_blunt(ptr)	((((ptr)->mflagsg & MG_RBLUNT) != 0L))
#define	resist_slash(ptr)	((((ptr)->mflagsg & MG_RSLASH) != 0L))
#define	resist_pierce(ptr)	((((ptr)->mflagsg & MG_RPIERCE) != 0L))
#define	resists_all(ptr)	((((ptr)->mflagsg & MG_RALL) == MG_RALL))

#define vulnerable_mask(mask)	((!((mask)&SLASH)+!((mask)&PIERCE)+!((mask)&WHACK)) == 1)

#define resists_poly(ptr)	(((ptr)->geno&G_UNIQ) \
							|| is_weeping(ptr) \
							|| (ptr)->mtyp == PM_VERMIURGE \
							|| is_yochlol(ptr))

#define resists_confusion(ptr)	(((ptr)->geno&G_UNIQ) || is_weeping(ptr) || is_yochlol(ptr))

#define is_blind(mon)		(!((mon)->mcansee) || (darksight((mon)->data) && !(\
													(!levl[(mon)->mx][(mon)->my].lit && !(viz_array[(mon)->my][(mon)->mx]&TEMP_LIT1 && !(viz_array[(mon)->my][(mon)->mx]&TEMP_DRK1)))\
													|| (levl[(mon)->mx][(mon)->my].lit &&  (viz_array[(mon)->my][(mon)->mx]&TEMP_DRK1 && !(viz_array[(mon)->my][(mon)->mx]&TEMP_LIT1))))))
#define is_deaf(mon)		(!((mon)->mcanhear) ||\
							  (mon)->mtyp == PM_NUPPERIBO ||\
							  (mon)->mtyp == PM_APHANACTONAN_ASSESSOR ||\
							  (mon)->mtyp == PM_ALABASTER_ELF ||\
							  (mon)->mtyp == PM_ALABASTER_ELF_ELDER)

#define has_template(mon, id)	((mon)->mtemplate == (id))
#define templated(mon)			((mon)->mtemplate != 0)
#define get_template(mon)			((mon)->mtemplate)

#define is_molochan(ptr)	((ptr)->maligntyp == MON_A_NONE)
#define is_voidalign(ptr)	((ptr)->maligntyp == MON_A_VOID)
#define is_lawful(ptr)		((ptr)->maligntyp > A_NEUTRAL && !is_molochan(ptr) && !is_voidalign(ptr))
#define is_lawful_mon(mon) (HAS_EPRI(mon) ? EPRI(mon)->shralign == A_LAWFUL :\
				  HAS_EMIN(mon) ? EMIN(mon)->min_align == A_LAWFUL :\
				  is_lawful((mon)->data))
#define is_neutral(ptr)		((ptr)->maligntyp == A_NEUTRAL)
#define is_neutral_mon(mon) (HAS_EPRI(mon) ? EPRI(mon)->shralign == A_NEUTRAL :\
				  HAS_EMIN(mon) ? EMIN(mon)->min_align == A_NEUTRAL :\
				  is_neutral((mon)->data))
#define is_chaotic(ptr)		((ptr)->maligntyp < A_NEUTRAL && !is_molochan(ptr) && !is_voidalign(ptr))
#define is_chaotic_mon(mon) (HAS_EPRI(mon) ? EPRI(mon)->shralign == A_CHAOTIC :\
				  HAS_EMIN(mon) ? EMIN(mon)->min_align == A_CHAOTIC :\
				  is_chaotic((mon)->data))

#define is_alabaster_mummy(ptr)	((ptr)->mtyp == PM_ALABASTER_MUMMY)

#define is_lminion(mon)		(is_minion((mon)->data) && is_lawful_mon(mon))

#define is_nminion(mon)		(is_minion((mon)->data) && is_neutral_mon(mon))

#define is_cminion(mon)		(is_minion((mon)->data) && is_chaotic_mon(mon))

#define notonline(ptr)			(((ptr)->mflagsm & MM_NOTONL) != 0L)
#define fleetflee(ptr)			(((ptr)->mflagsm & MM_FLEETFLEE) != 0L)
#define bold(ptr)				(((ptr)->mflagst & MT_BOLD) != 0L)
#define is_clinger(ptr)			(((ptr)->mflagsm & MM_CLING) != 0L)
#define species_flies(ptr)		(((ptr)->mflagsm & MM_FLY) != 0L)
#define species_displaces(ptr)	(((ptr)->mflagsg & MG_DISPLACEMENT) != 0L)
#define species_floats(ptr)		(((ptr)->mflagsm & MM_FLOAT) != 0L)
#define species_swims(ptr)		(((ptr)->mflagsm & MM_SWIM) != 0L)
#define species_tears_webs(ptr)		(((ptr)->mflagsm & MM_WEBRIP) != 0L)
#define species_busts_doors(ptr)		(((ptr)->mflagsm & MM_DOORBUST) != 0L)
#define is_suicidal(ptr)		(is_fern_spore(ptr) || \
					(ptr)->mtyp == PM_FREEZING_SPHERE || \
					(ptr)->mtyp == PM_FLAMING_SPHERE || \
					(ptr)->mtyp == PM_SHOCKING_SPHERE)
#define breathless(ptr)			(((ptr)->mflagsm & MM_BREATHLESS) != 0L)
#define breathless_mon(mon)		(breathless((mon)->data) || mon_resistance((mon), MAGICAL_BREATHING))
#define amphibious(ptr)			(((ptr)->mflagsm & (MM_AMPHIBIOUS | MM_BREATHLESS)) != 0L)
#define amphibious_mon(mon)		(amphibious((mon)->data) || mon_resistance((mon), MAGICAL_BREATHING) || mon_resistance((mon), SWIMMING))
#define species_passes_walls(ptr)	(((ptr)->mflagsm & MM_WALLWALK) != 0L)
#define amorphous(ptr)			(((ptr)->mflagsm & MM_AMORPHOUS) != 0L)
#define noncorporeal(ptr)		((ptr)->mlet == S_GHOST || (ptr)->mlet == S_SHADE)
#define insubstantial(ptr)		(((ptr)->mflagsb & MB_INSUBSTANTIAL) != 0L)
#define tunnels(ptr)			(((ptr)->mflagsm & MM_TUNNEL) != 0L)
#define needspick(ptr)			(((ptr)->mflagsm & MM_NEEDPICK) != 0L)
#define is_underswimmer(ptr)	((ptr)->mlet == S_EEL || (ptr)->mtyp == PM_CRYSTAL_OOZE)
#define hides_under(ptr)		(((ptr)->mflagst & MT_CONCEAL) != 0L)
#define is_hider(ptr)			(((ptr)->mflagst & MT_HIDE) != 0L)
#define is_backstabber(ptr)		(((ptr)->mflagsg & MG_BACKSTAB) != 0L)
#define is_commander(ptr)		(((ptr)->mflagsg & MG_COMMANDER) != 0L)
/*#define haseyes(ptr)			(((ptr)->mflagsb & MB_NOEYES) == 0L) when did this get duplicated???*/
#define haseyes(ptr)			(((ptr)->mflagsb & MB_NOEYES) == 0L)
#define nomouth(mtyp)			(mtyp==PM_NIGHTGAUNT || mtyp==PM_STRANGER || mtyp==PM_SUZERAIN || mtyp==PM_CARCOSAN_COURTIER)
#define goodsmeller(ptr)		(((ptr)->mflagsv & MV_SCENT) != 0L)
#define is_tracker(ptr)			(((ptr)->mflagsg & MG_TRACKER) != 0L)
#define eyecount(ptr)			(!haseyes(ptr) ? 0 : \
				 ((ptr)->mtyp == PM_CYCLOPS || \
				  (ptr)->mtyp == PM_MONOTON || \
				  (ptr)->mtyp == PM_FLOATING_EYE) ? 1 : 2)
#define sensitive_ears(ptr)		(((ptr)->mflagsv & MV_ECHOLOCATE) != 0L || (ptr)->mtyp == PM_APHANACTONAN_AUDIENT)
#define nohands(ptr)		(((ptr)->mflagsb & (MB_NOHANDS|MB_NOLIMBS)) != 0L)
#define nolimbs(ptr)		(((ptr)->mflagsb & MB_NOLIMBS) == MB_NOLIMBS)
#define nofeet(ptr)			((ptr)->mflagsb & MB_NOFEET)
#define nogloves(ptr)		(nohands(ptr) || ((ptr)->mflagsb & MB_NOGLOVES))
#define nohat(ptr)			(!has_head(ptr) || ((ptr)->mflagsb & MB_NOHAT))
#define noshield(ptr)		((nogloves(ptr) && !allow_shield(ptr)) || ptr->mtyp == PM_CARCOSAN_COURTIER)
#define notake(ptr)		(((ptr)->mflagst & MT_NOTAKE) != 0L)
#define has_head(ptr)		(((ptr)->mflagsb & MB_NOHEAD) == 0L)
#define has_head_mon(mon) ((mon == &youmonst) ? (has_head(youracedata)) : (has_head((mon)->data)))
#define has_horns(ptr)		(num_horns(ptr) > 0)
#define is_whirly(ptr)		((ptr)->mlet == S_VORTEX || \
				 (ptr)->mtyp == PM_AIR_ELEMENTAL ||\
				 (ptr)->mtyp == PM_ILLURIEN_OF_THE_MYRIAD_GLIMPSES ||\
				 (ptr)->mtyp == PM_DREADBLOSSOM_SWARM)
#define has_passthrough_displacement(ptr)	((ptr)->mtyp == PM_WRAITHWORM ||\
				 (ptr)->mtyp == PM_FIRST_WRAITHWORM)
#define flaming(ptr)		((ptr)->mtyp == PM_FIRE_VORTEX || \
				 (ptr)->mtyp == PM_FLAMING_SPHERE || \
				 (ptr)->mtyp == PM_FIRE_ELEMENTAL || \
				 (ptr)->mtyp == PM_FIRE_STORM || \
				 (ptr)->mtyp == PM_FLAMING_ORB || \
				 (ptr)->mtyp == PM_HELLFIRE_COLOSSUS || \
				 (ptr)->mtyp == PM_HELLFIRE_ORB || \
				 (ptr)->mtyp == PM_DANCING_FLAME || \
				 (ptr)->mtyp == PM_BALL_OF_GOSSAMER_SUNLIGHT || \
				 (ptr)->mtyp == PM_ANCIENT_OF_THE_BURNING_WASTES || \
				 (ptr)->mtyp == PM_FIERNA || \
				 (ptr)->mtyp == PM_MOLEK || \
				 (ptr)->mtyp == PM_SALAMANDER)
#define is_gold(ptr)	((ptr)->mtyp == PM_GOLD_GOLEM || \
				 (ptr)->mtyp == PM_GOLDEN_HEART || \
				 (ptr)->mtyp == PM_TREASURY_GOLEM || \
				 (ptr)->mtyp == PM_AURUMACH_RILMANI || \
				 (ptr)->mtyp == PM_ARA_KAMEREL || \
				 (ptr)->mtyp == PM_ACERERAK || \
				 (ptr)->mtyp == PM_RADIANT_PYRAMID)
#define is_iron(ptr)	((ptr)->mtyp == PM_IRON_PIERCER || \
				 (ptr)->mtyp == PM_IRON_GOLEM || \
				 (ptr)->mtyp == PM_GREEN_STEEL_GOLEM || \
				 (ptr)->mtyp == PM_CHAIN_GOLEM || \
				 (ptr)->mtyp == PM_SCRAP_TITAN || \
				 (ptr)->mtyp == PM_HELLFIRE_COLOSSUS || \
				 (ptr)->mtyp == PM_HELLFIRE_ORB || \
				 (ptr)->mtyp == PM_FERRUMACH_RILMANI)
#define is_iron_mon(mon)	(is_iron((mon)->data))
#define is_silver(ptr)	((ptr)->mtyp == PM_ARGENACH_RILMANI || \
				 (ptr)->mtyp == PM_AMM_KAMEREL || \
				 (ptr)->mtyp == PM_ARGENTUM_GOLEM)
#define is_silver_mon(mon)	(is_silver((mon)->data) || has_template(mon, CONSTELLATION))
#define is_stone(ptr)	((ptr)->mtyp == PM_DUST_VORTEX || \
				 (ptr)->mtyp == PM_EARTH_ELEMENTAL || \
				 (ptr)->mtyp == PM_TERRACOTTA_SOLDIER || \
				 (ptr)->mtyp == PM_STONE_GOLEM || \
				 (ptr)->mtyp == PM_SENTINEL_OF_MITHARDIR || \
				 (ptr)->mtyp == PM_GARGOYLE || \
				 (ptr)->mtyp == PM_WINGED_GARGOYLE || \
				 (ptr)->mtyp == PM_XORN)
#define is_anhydrous(ptr)	(flaming(ptr)  || \
							 is_clockwork(ptr) || \
							 is_stone(ptr) || \
							 is_auton(ptr) || \
				 (ptr)->mlet == S_KETER || \
				 (ptr)->mtyp == PM_AOA || \
				 (ptr)->mtyp == PM_AOA_DROPLET)
#define is_watery(ptr)	((ptr)->mtyp == PM_WATER_ELEMENTAL \
				 || (ptr)->mtyp == PM_FORD_ELEMENTAL \
				 || (ptr)->mtyp == PM_WATER_DOLPHIN \
				 || (ptr)->mtyp == PM_WATERSPOUT \
				 || (ptr)->mtyp == PM_UISCERRE_ELADRIN \
				 || (ptr)->mtyp == PM_FOG_CLOUD \
				 || (ptr)->mtyp == PM_STEAM_VORTEX \
				 || (ptr)->mtyp == PM_ANCIENT_TEMPEST \
				 || (ptr)->mtyp == PM_MORTAI \
				 || (ptr)->mtyp == PM_HUDOR_KAMEREL \
				 || (ptr)->mtyp == PM_LETHE_ELEMENTAL \
				 || (ptr)->mtyp == PM_DAUGHTER_OF_NAUNET \
				 )
#define is_uvuudaum(ptr)	((ptr)->mtyp == PM_UVUUDAUM \
				 || (ptr)->mtyp == PM_MASKED_QUEEN \
				 )
#define is_witch_mon(mon)	((mon)->mtyp == PM_APPRENTICE_WITCH \
				 || (mon)->mtyp == PM_WITCH \
				 || (mon)->mtyp == PM_COVEN_LEADER \
				 )
#define removed_innards(ptr)	(((ptr)->mtyp == PM_HUNGRY_DEAD) || \
						 ((ptr)->mtyp == PM_KOBOLD_MUMMY) || \
						 ((ptr)->mtyp == PM_GNOME_MUMMY) || \
						 ((ptr)->mtyp == PM_ORC_MUMMY) || \
						 ((ptr)->mtyp == PM_DWARF_MUMMY) || \
						 ((ptr)->mtyp == PM_ELF_MUMMY) || \
						 ((ptr)->mtyp == PM_HUMAN_MUMMY) || \
						 ((ptr)->mtyp == PM_HALF_DRAGON_MUMMY) || \
						 ((ptr)->mtyp == PM_ETTIN_MUMMY) || \
						 ((ptr)->mtyp == PM_CHIROPTERAN_MUMMY) || \
						 ((ptr)->mtyp == PM_GIANT_MUMMY) || \
						 ((ptr)->mtyp == PM_SHAMBLING_HORROR && u.shambin == 3) || \
						 ((ptr)->mtyp == PM_STUMBLING_HORROR && u.stumbin == 3) || \
						 ((ptr)->mtyp == PM_WANDERING_HORROR && u.wandein == 3) || \
						 ((ptr)->mtyp == PM_NITOCRIS) || \
						 ((ptr)->mtyp == PM_PHARAOH) \
						)
#define skeleton_innards(ptr)	(((ptr)->mtyp == PM_SKELETON) || \
						 ((ptr)->mtyp == PM_SKELETAL_PIRATE) \
						)
#define no_innards(ptr)	((ptr)->mlet == S_VORTEX || \
						 (ptr)->mlet == S_LIGHT || \
						 (ptr)->mlet == S_ELEMENTAL || \
						 (ptr)->mlet == S_MIMIC || \
						 ((ptr)->mtyp == PM_BALL_OF_LIGHT) || \
						 ((ptr)->mtyp == PM_BALL_OF_RADIANCE) || \
						 ((ptr)->mtyp == PM_BALL_OF_GOSSAMER_SUNLIGHT) || \
						 ((ptr)->mtyp == PM_DAUGHTER_OF_NAUNET) || \
						 ((ptr)->mtyp == PM_SHAMBLING_HORROR && u.shambin == 2) || \
						 ((ptr)->mtyp == PM_STUMBLING_HORROR && u.stumbin == 2) || \
						 ((ptr)->mtyp == PM_WANDERING_HORROR && u.wandein == 2) || \
						 (ptr)->mlet == S_WRAITH || \
						 (ptr)->mlet == S_GHOST || \
						 (ptr)->mlet == S_SHADE || \
						 (ptr)->mlet == S_GOLEM \
						)
#define undiffed_innards(ptr)	((ptr)->mlet == S_BLOB || \
								 (ptr)->mtyp == PM_FLOATING_EYE || \
								 (ptr)->mlet == S_JELLY || \
								 (ptr)->mlet == S_TRAPPER || \
								 (ptr)->mlet == S_FUNGUS || \
								 (ptr)->mlet == S_PUDDING || \
								 ((ptr)->mtyp == PM_DROW_MUMMY) || \
								 ((ptr)->mtyp == PM_SHAMBLING_HORROR && u.shambin == 1) || \
								 ((ptr)->mtyp == PM_STUMBLING_HORROR && u.stumbin == 1) || \
								 ((ptr)->mtyp == PM_WANDERING_HORROR && u.wandein == 1) || \
								 (ptr)->mlet == S_PLANT \
								)
#define is_silent(ptr)		((ptr)->msound == MS_SILENT)
#define is_silent_mon(mon)	(is_silent((mon)->data))
#define unsolid(ptr)		(((ptr)->mflagsb & MB_UNSOLID) != 0L)
#define mindless(ptr)		(((ptr)->mflagst & MT_MINDLESS) != 0L || on_level(&valley_level, &u.uz))
#define mindless_mon(mon)		(mon && mindless((mon)->data))
#define intelligent_mon(mon)	(!mindless_mon(mon) && !is_animal((mon)->data))
#define murderable_mon(mon)	((mon) && ((intelligent_mon(mon) && always_peaceful((mon)->data) && !always_hostile_mon(mon)) || (mon)->isshk || (mon)->isgd || (mon)->ispriest))

#define mortal_race(mon)	(intelligent_mon(mon) && !nonliving((mon)->data) && !is_minion((mon)->data) && !is_demon((mon)->data) && !is_primordial((mon)->data) && !is_great_old_one((mon)->data))
#define dark_immune(mon)	(is_unalive((mon)->data) || is_primordial((mon)->data))

#define slithy(ptr)			((ptr)->mflagsb & MB_SLITHY)
#define humanoid_torso(ptr)	(((ptr)->mflagsb & MB_HUMANOID) != 0)
#define humanoid_upperbody(ptr)	(humanoid_torso(ptr) && (((ptr)->mflagsb&(MB_LONGHEAD|MB_LONGNECK)) == 0))
#define humanoid_feet(ptr)	(((ptr)->mflagsb & MB_HAS_FEET) != 0)
#define humanoid(ptr)		(((ptr)->mflagsb & MB_BODYTYPEMASK) == MB_HUMANOID)
#define animaloid(ptr)		(((ptr)->mflagsb & MB_BODYTYPEMASK) == MB_ANIMAL)
#define serpentine(ptr)		(((ptr)->mflagsb & MB_BODYTYPEMASK) == MB_SLITHY)
#define centauroid(ptr)		(((ptr)->mflagsb & MB_BODYTYPEMASK) == (MB_HUMANOID|MB_ANIMAL))
#define snakemanoid(ptr)	(((ptr)->mflagsb & MB_BODYTYPEMASK) == (MB_HUMANOID|MB_SLITHY))
#define leggedserpent(ptr)	(((ptr)->mflagsb & MB_BODYTYPEMASK) == (MB_ANIMAL|MB_SLITHY))
#define naoid(ptr)			(((ptr)->mflagsb & MB_BODYTYPEMASK) == 0)

#define noanatomy(ptr)			(noncorporeal(ptr) || amorphous(ptr) || naoid(ptr))

#define noboots(ptr)			((slithy(ptr) || nolimbs(ptr) || nofeet(ptr)) && !humanoid_feet(ptr))

#define has_wings(ptr)			(((ptr)->mflagsb & MB_WINGS) != 0)

#define is_animal(ptr)		(((ptr)->mflagst & MT_ANIMAL) != 0L)
#define is_plant(ptr)		(((ptr)->mflagsa & MA_PLANT) != 0L)
#define is_insectoid(ptr)		(((ptr)->mflagsa & MA_INSECTOID) != 0L)
#define is_arachnid(ptr)		(((ptr)->mflagsa & MA_ARACHNID) != 0L)
#define is_aquatic(ptr)		(((ptr)->mflagsa & MA_AQUATIC) != 0L)
#define is_wooden(ptr)		((ptr)->mtyp == PM_WOOD_GOLEM || (ptr)->mtyp == PM_LIVING_LECTERN || is_plant(ptr))
#define thick_skinned(ptr)	(((ptr)->mflagsb & MB_THICK_HIDE) != 0L)
#define lays_eggs(ptr)		(((ptr)->mflagsb & MB_OVIPAROUS) != 0L)
#define species_regenerates(ptr)		(((ptr)->mflagsg & MG_REGEN) != 0L)
#define species_perceives(ptr)			(((ptr)->mflagsv & MV_SEE_INVIS) != 0L)
#define species_teleports(ptr)			(((ptr)->mflagsm & MM_TPORT) != 0L)
#define species_controls_teleports(ptr)	(((ptr)->mflagsm & MM_TPORT_CNTRL) != 0L)
#define species_is_telepathic(ptr)		(((ptr)->mflagsv & MV_TELEPATHIC) != 0L)
#define is_armed(ptr)		(attacktype(ptr, AT_WEAP) || attacktype(ptr, AT_XWEP) || attacktype(ptr, AT_MARI) || attacktype(ptr, AT_DEVA))
#define is_armed_mon(mon)	(mon_attacktype(mon, AT_WEAP) || mon_attacktype(mon, AT_XWEP) || mon_attacktype(mon, AT_MARI) || mon_attacktype(mon, AT_DEVA))
#define crpsdanger(ptr)		(acidic(ptr) || poisonous(ptr) ||\
							 freezing(ptr) || burning(ptr))
#define hideablewidegaze(ptr)	((ptr)->mtyp == PM_MEDUSA || \
								 (ptr)->mtyp == PM_GREAT_CTHULHU || \
								 (ptr)->mtyp == PM_STAR_SPAWN || \
								 (ptr)->mtyp == PM_DAGON || \
								 (ptr)->mtyp == PM_PALE_NIGHT || \
								 (ptr)->mtyp == PM_OBOX_OB || \
								 (ptr)->mtyp == PM_UVUUDAUM || \
								 (ptr)->mtyp == PM_MASKED_QUEEN \
								 )
#define controlledwidegaze(ptr)		(!((ptr)->mtyp == PM_MEDUSA || (ptr)->mtyp == PM_UVUUDAUM || (ptr)->mtyp == PM_GREAT_CTHULHU || (ptr)->mtyp == PM_OBOX_OB || (ptr)->mtyp == PM_DAGON))
#define controlledwidegaze_mon(mon)		(controlledwidegaze((mon)->data) || has_template(mon, ILLUMINATED))
#define acidic(ptr)			(((ptr)->mflagsb & MB_ACID) != 0L)
#define poisonous(ptr)		(((ptr)->mflagsb & MB_POIS) != 0L)
#define freezing(ptr)		(((ptr)->mflagsb & MB_CHILL) != 0L)
#define burning(ptr)		(((ptr)->mflagsb & MB_TOSTY) != 0L)
#define hallucinogenic(ptr)		(((ptr)->mflagsb & MB_HALUC) != 0L)
#define inediate(ptr)		(!(carnivorous(ptr) || herbivorous(ptr) || metallivorous(ptr) || magivorous(ptr) || is_vampire(ptr)))
#define carnivorous(ptr)	(((ptr)->mflagst & MT_CARNIVORE) != 0L)
#define herbivorous(ptr)	(((ptr)->mflagst & MT_HERBIVORE) != 0L)
#define metallivorous(ptr)	(((ptr)->mflagst & MT_METALLIVORE) != 0L)
#define magivorous(ptr)		(((ptr)->mflagst & MT_MAGIVORE) != 0L)
#define polyok(ptr)			((((ptr)->mflagsg & MG_NOPOLY) == 0L) && (G_C_INST((ptr)->geno) <= u.uinsight))
#define is_Rebel(ptr)		((ptr)->mtyp == PM_REBEL_RINGLEADER ||\
							 (ptr)->mtyp == PM_ADVENTURING_WIZARD ||\
							 (ptr)->mtyp == PM_MILITANT_CLERIC ||\
							 (ptr)->mtyp == PM_HALF_ELF_RANGER)
#define is_undead(ptr)		(((ptr)->mflagsa & MA_UNDEAD) != 0L)
#define	can_undead(ptr)	(!nonliving(ptr) && !is_minion(ptr) && ((ptr)->mlet != S_PUDDING) &&\
								((ptr)->mlet != S_JELLY) && ((ptr)->mlet != S_BLOB) && !is_elemental(ptr) &&\
								!is_plant(ptr) && !is_demon(ptr) && !is_great_old_one(ptr) && !is_primordial(ptr) && !(mvitals[monsndx(ptr)].mvflags&G_NOCORPSE))
#define is_weldproof(ptr)		(is_undead(ptr) || is_demon(ptr) || is_were(ptr) || is_great_old_one(ptr) || ptr->mtyp == PM_DAUGHTER_OF_NAUNET || ptr->mtyp == PM_PARASITIC_WALL_HUGGER)
#define is_weldproof_mon(mon)		(is_weldproof((mon)->data))
#define is_were(ptr)		(((ptr)->mflagsa & MA_WERE) != 0L)
#define is_heladrin(ptr)		(\
							 (ptr)->mtyp == PM_COURE_ELADRIN || \
							 (ptr)->mtyp == PM_NOVIERE_ELADRIN || \
							 (ptr)->mtyp == PM_BRALANI_ELADRIN || \
							 (ptr)->mtyp == PM_FIRRE_ELADRIN || \
							 (ptr)->mtyp == PM_SHIERE_ELADRIN || \
							 (ptr)->mtyp == PM_GHAELE_ELADRIN || \
							 (ptr)->mtyp == PM_TULANI_ELADRIN || \
							 (ptr)->mtyp == PM_GAE_ELADRIN || \
							 (ptr)->mtyp == PM_BRIGHID_ELADRIN || \
							 (ptr)->mtyp == PM_UISCERRE_ELADRIN || \
							 (ptr)->mtyp == PM_CAILLEA_ELADRIN || \
							 (ptr)->mtyp == PM_DRACAE_ELADRIN || \
							 (ptr)->mtyp == PM_ALRUNES ||\
							 (ptr)->mtyp == PM_GWYNHARWYF ||\
							 (ptr)->mtyp == PM_ASCODEL ||\
							 (ptr)->mtyp == PM_FAERINAAL ||\
							 (ptr)->mtyp == PM_QUEEN_MAB ||\
							 (ptr)->mtyp == PM_QUEEN_OF_STARS ||\
							 (ptr)->mtyp == PM_KETO \
							)
#define is_eeladrin(ptr)	(\
							 (ptr)->mtyp == PM_MOTE_OF_LIGHT || \
							 (ptr)->mtyp == PM_WATER_DOLPHIN || \
							 (ptr)->mtyp == PM_SINGING_SAND || \
							 (ptr)->mtyp == PM_DANCING_FLAME || \
							 (ptr)->mtyp == PM_BALL_OF_LIGHT || \
							 (ptr)->mtyp == PM_LUMINOUS_CLOUD || \
							 (ptr)->mtyp == PM_BALL_OF_RADIANCE || \
							 (ptr)->mtyp == PM_WARDEN_TREE || \
							 (ptr)->mtyp == PM_PYROCLASTIC_VORTEX || \
							 (ptr)->mtyp == PM_WATERSPOUT || \
							 (ptr)->mtyp == PM_MOONSHADOW || \
							 (ptr)->mtyp == PM_MOTHERING_MASS || \
							 (ptr)->mtyp == PM_HATEFUL_WHISPERS ||\
							 (ptr)->mtyp == PM_FURIOUS_WHIRLWIND ||\
							 (ptr)->mtyp == PM_BLOODY_SUNSET ||\
							 (ptr)->mtyp == PM_BALL_OF_GOSSAMER_SUNLIGHT ||\
							 (ptr)->mtyp == PM_COTERIE_OF_MOTES ||\
							 (ptr)->mtyp == PM_ETERNAL_LIGHT ||\
							 (ptr)->mtyp == PM_ANCIENT_TEMPEST \
							)
#define is_yochlol(ptr)		((ptr)->mtyp == PM_YOCHLOL ||\
							 (ptr)->mtyp == PM_UNEARTHLY_DROW ||\
							 (ptr)->mtyp == PM_STINKING_CLOUD ||\
							 (ptr)->mtyp == PM_DEMONIC_BLACK_WIDOW)
#define is_duergar(ptr)		((ptr)->mtyp == PM_DUERGAR ||\
							 (ptr)->mtyp == PM_GIANT_DUERGAR ||\
							 (ptr)->mtyp == PM_DUERGAR_DEEPKING||\
							 (ptr)->mtyp == PM_GIANT_DEEPKING ||\
							 (ptr)->mtyp == PM_DUERGAR_STONEGUARD ||\
							 (ptr)->mtyp == PM_GIANT_STONEGUARD ||\
							 (ptr)->mtyp == PM_DUERGAR_DEBILITATOR ||\
							 (ptr)->mtyp == PM_DUERGAR_ANNIHILATOR)
#define is_kuo_toa(ptr)		((ptr)->mtyp == PM_KUO_TOA ||\
							 (ptr)->mtyp == PM_KUO_TOA_WHIP)
#define is_vampire(ptr)		(((ptr)->mflagsa & MA_VAMPIRE) != 0L)
#define is_half_dragon(ptr)		attacktype_fordmg(ptr, AT_BREA, AD_HDRG)
#define is_boreal_dragoon(ptr)		(attacktype_fordmg(ptr, AT_WEAP, AD_HDRG) || attacktype_fordmg(ptr, AT_XWEP, AD_HDRG))
#define is_elf(ptr)			(((ptr)->mflagsa & MA_ELF) != 0L && !is_drow(ptr))
#define is_drow(ptr)		(((ptr)->mflagsa & MA_DROW) != 0L)
#define is_myrkalfr(ptr)	((ptr)->mtyp == PM_MYRKALFAR_WARRIOR || (ptr)->mtyp == PM_MYRKALFAR_MATRON || (ptr)->mtyp == PM_MYRKALFR || (ptr)->mtyp == PM_ALIDER)
#define is_dwarf(ptr)		(((ptr)->mflagsa & MA_DWARF) != 0L)
#define is_gnome(ptr)		(((ptr)->mflagsa & MA_GNOME) != 0L)
#define is_gizmo(ptr)		((ptr)->mlet == S_GNOME && is_clockwork(ptr))
#define is_szcultist(ptr)		((ptr)->mtyp == PM_SHATTERED_ZIGGURAT_CULTIST \
								|| (ptr)->mtyp == PM_SHATTERED_ZIGGURAT_KNIGHT \
								|| (ptr)->mtyp == PM_SHATTERED_ZIGGURAT_WIZARD)
#define is_orc(ptr)		(((ptr)->mflagsa & MA_ORC) != 0L)
#define is_ogre(ptr)		((ptr)->mlet == S_OGRE)
#define is_troll(ptr)		((ptr)->mlet == S_TROLL)
#define is_kobold(ptr)		((ptr)->mlet == S_KOBOLD)
#define is_ettin(ptr)		((ptr)->mtyp == PM_ETTIN)
#define is_human(ptr)		(((ptr)->mflagsa & MA_HUMAN) != 0L)
#define is_untamable(ptr)	(((ptr)->mflagsg & MG_NOTAME) != 0L)
#define is_unwishable(ptr)	((((ptr)->mflagsg & MG_NOWISH) != 0L) || (G_C_INST((ptr)->geno) > u.uinsight) || ((((ptr)->mflagsg&MG_FUTURE_WISH) != 0L) && !Role_if(PM_TOURIST)))
#define is_fungus(ptr)		((ptr)->mlet == S_FUNGUS)
#define is_migo(ptr)		((ptr)->mtyp == PM_MIGO_WORKER ||\
							 (ptr)->mtyp == PM_MIGO_SOLDIER ||\
							 (ptr)->mtyp == PM_MIGO_PHILOSOPHER ||\
							 (ptr)->mtyp == PM_MIGO_QUEEN)
#define your_race(ptr)		(((ptr)->mflagsa & urace.selfmask) != 0L)
#define is_andromaliable(ptr)	(is_elf(ptr) || is_drow(ptr) || is_dwarf(ptr) || is_gnome(ptr) || is_orc(ptr) || is_human(ptr) || (ptr)->mtyp == PM_HOBBIT || \
								 (ptr)->mtyp == PM_MONKEY || (ptr)->mtyp == PM_APE || (ptr)->mtyp == PM_YETI || \
								 (ptr)->mtyp == PM_CARNIVOROUS_APE || (ptr)->mtyp == PM_SASQUATCH\
								)
#define is_bat(ptr)		((ptr)->mtyp == PM_BAT || \
				 (ptr)->mtyp == PM_GIANT_BAT || \
				 (ptr)->mtyp == PM_BATTLE_BAT || \
				 (ptr)->mtyp == PM_WARBAT || \
				 (ptr)->mtyp == PM_VAMPIRE_BAT)
#define is_metroid(ptr) ((ptr)->mlet == S_TRAPPER && !((ptr)->mtyp == PM_TRAPPER || (ptr)->mtyp == PM_LURKER_ABOVE))
#define is_social_insect(ptr) ((ptr)->mlet == S_ANT && (ptr)->maligntyp > 0)
#define is_spider(ptr)	((ptr)->mlet == S_SPIDER && (\
				 (ptr)->mtyp == PM_CAVE_SPIDER ||\
				 (ptr)->mtyp == PM_GIANT_SPIDER ||\
				 (ptr)->mtyp == PM_MIRKWOOD_SPIDER ||\
				 (ptr)->mtyp == PM_PHASE_SPIDER ||\
				 (ptr)->mtyp == PM_MIRKWOOD_ELDER \
				 ))
#define is_rat(ptr)		((ptr)->mlet == S_RODENT && ( \
				 (ptr)->mtyp == PM_SEWER_RAT || \
				 (ptr)->mtyp == PM_GIANT_RAT || \
				 (ptr)->mtyp == PM_RABID_RAT || \
				 (ptr)->mtyp == PM_ENORMOUS_RAT || \
				 (ptr)->mtyp == PM_RODENT_OF_UNUSUAL_SIZE))
#define is_basic_worm(ptr)	((ptr)->mlet == S_WORM && (\
				 (ptr)->mtyp == PM_DARK_WORM ||\
				 (ptr)->mtyp == PM_BABY_LONG_WORM ||\
				 (ptr)->mtyp == PM_BABY_PURPLE_WORM ||\
				 (ptr)->mtyp == PM_LONG_WORM ||\
				 (ptr)->mtyp == PM_PURPLE_WORM \
				 ))
#define is_wormy_thing(ptr)	(((ptr)->mlet == S_EEL && (\
				 (ptr)->mtyp == PM_GIANT_EEL ||\
				 (ptr)->mtyp == PM_ELECTRIC_EEL ||\
				 (ptr)->mtyp == PM_KRAKEN \
				)) || ((ptr)->mlet == S_SPIDER && (\
				 (ptr)->mtyp == PM_CENTIPEDE \
				)) || ((ptr)->mlet == S_SNAKE && (\
				 (ptr)->mtyp == PM_GARTER_SNAKE ||\
				 (ptr)->mtyp == PM_SNAKE ||\
				 (ptr)->mtyp == PM_WATER_MOCCASIN ||\
				 (ptr)->mtyp == PM_PIT_VIPER ||\
				 (ptr)->mtyp == PM_PYTHON ||\
				 (ptr)->mtyp == PM_COBRA ||\
				 (ptr)->mtyp == PM_DAUGHTER_OF_NAUNET \
				 ))\
			)
#define is_wormy_dragon(ptr)	((ptr)->mlet == S_DRAGON && (\
				 (ptr)->mtyp == PM_GRAY_DRAGON ||\
				 (ptr)->mtyp == PM_SILVER_DRAGON ||\
				 (ptr)->mtyp == PM_SHIMMERING_DRAGON ||\
				 (ptr)->mtyp == PM_WHITE_DRAGON ||\
				 (ptr)->mtyp == PM_ORANGE_DRAGON ||\
				 (ptr)->mtyp == PM_BLACK_DRAGON ||\
				 (ptr)->mtyp == PM_BLUE_DRAGON ||\
				 (ptr)->mtyp == PM_GREEN_DRAGON ||\
				 (ptr)->mtyp == PM_RED_DRAGON ||\
				 (ptr)->mtyp == PM_DEEP_DRAGON ||\
				 (ptr)->mtyp == PM_YELLOW_DRAGON\
				 ))
#define is_dragon(ptr)		(((ptr)->mflagsa & MA_DRAGON) != 0L)
#define is_true_dragon(ptr)		((monsndx(ptr) >= PM_BABY_GRAY_DRAGON && monsndx(ptr) <= PM_YELLOW_DRAGON) || \
								(ptr)->mtyp == PM_PLATINUM_DRAGON || (ptr)->mtyp == PM_CHROMATIC_DRAGON)
#define is_true_adult_dragon(ptr)		((monsndx(ptr) >= PM_GRAY_DRAGON && monsndx(ptr) <= PM_YELLOW_DRAGON) || \
								(ptr)->mtyp == PM_PLATINUM_DRAGON || (ptr)->mtyp == PM_CHROMATIC_DRAGON || (ptr)->mtyp == PM_IXOTH || (ptr)->mtyp == PM_SMAUG)
#define is_pseudodragon(ptr)	(monsndx(ptr) >= PM_TINY_PSEUDODRAGON && monsndx(ptr) <= PM_GIGANTIC_PSEUDODRAGON)
#define is_bird(ptr)		(((ptr)->mflagsa & MA_AVIAN) != 0L)
#define is_giant(ptr)		(((ptr)->mflagsa & MA_GIANT) != 0L)
#define is_gnoll(ptr)		((ptr)->mtyp == PM_GNOLL || \
				 (ptr)->mtyp == PM_GNOLL_GHOUL || \
				 (ptr)->mtyp == PM_ANUBITE || \
				 (ptr)->mtyp == PM_GNOLL_MATRIARCH || \
				 (ptr)->mtyp == PM_YEENOGHU)
#define is_minotaur(ptr)		((ptr)->mtyp == PM_MINOTAUR || \
				 (ptr)->mtyp == PM_MINOTAUR_PRIESTESS || \
				 (ptr)->mtyp == PM_BAPHOMET)
#define is_pirate(ptr)	((ptr)->mtyp == PM_PIRATE || \
				 (ptr)->mtyp == PM_PIRATE_BROTHER || \
				 (ptr)->mtyp == PM_SKELETAL_PIRATE || \
				 (ptr)->mtyp == PM_DAMNED_PIRATE || \
				 (ptr)->mtyp == PM_GITHYANKI_PIRATE || \
				 (ptr)->mtyp == PM_MAYOR_CUMMERBUND)
#define is_golem(ptr)		((ptr)->mlet == S_GOLEM)
#define is_clockwork(ptr)	(((ptr)->mflagsa & MA_CLOCK) != 0L)
#define is_domestic(ptr)	(((ptr)->mflagst & MT_DOMESTIC) != 0L)
#define is_fey(ptr)			(((ptr)->mflagsa & MA_FEY) != 0L)
#define is_snake(ptr)		(((ptr)->mflagsa & MA_REPTILIAN) != 0L && (ptr)->mlet == S_SNAKE)
#define is_demon(ptr)		(((ptr)->mflagsa & MA_DEMON) != 0L)
#define is_law_demon(ptr)	(((ptr)->mflagsa & MA_DEMON) != 0L && is_lawful(ptr))
#define is_cha_demon(ptr)	(((ptr)->mflagsa & MA_DEMON) != 0L && is_chaotic(ptr))
#define is_ancient(ptr)		(((ptr)->mtyp >=  PM_ANCIENT_OF_BLESSINGS\
							&& (ptr)->mtyp <= PM_ANCIENT_OF_DEATH) \
							|| (ptr)->mtyp == PM_BAALPHEGOR \
							)
#define is_tannin(ptr)		(((ptr)->mtyp >= PM_AKKABISH_TANNIN \
							  && (ptr)->mtyp <= PM_TERAPHIM_TANNAH \
							 ) \
							|| (ptr)->mtyp == PM_PALE_NIGHT \
							|| (ptr)->mtyp == PM_DAGON \
							|| (ptr)->mtyp == PM_OBOX_OB \
							)
#define is_primordial(ptr)	(((ptr)->mflagsa & MA_PRIMORDIAL) != 0L)
#define is_great_old_one(ptr)	(((ptr)->mflagsa & MA_G_O_O) != 0L)
#define is_mask_of_nyarlathotep(ptr)	((ptr)->mtyp == PM_GOOD_NEIGHBOR || (ptr)->mtyp == PM_HMNYW_PHARAOH || (ptr)->mtyp == PM_NITOCRIS)
#define is_alien(ptr)	(((ptr)->mflagsa & MA_ET) != 0L)
#define is_keter(ptr)		((ptr)->mlet == S_KETER)
#define is_angel(ptr)		((((ptr)->mflagsa & MA_MINION) != 0L) && ((ptr)->mlet == S_LAW_ANGEL || (ptr)->mlet == S_NEU_ANGEL || (ptr)->mlet == S_CHA_ANGEL))
#define fallen(mx) 			(has_template(mx, MAD_TEMPLATE) || has_template(mx, FALLEN_TEMPLATE) || mx->mfaction == LAMASHTU_FACTION)
#define normalAngel(mx) 	((is_angel(mx->data) && !fallen(mx)) || (is_undead(mx->data) && mx->mfaction == HOLYDEAD_FACTION))
#define fallenAngel(mx) 	(is_angel(mx->data) && fallen(mx))
#define is_eladrin(ptr)		(is_heladrin(ptr) || is_eeladrin(ptr))
#define is_high_caste_eladrin(ptr)		(\
							 (ptr)->mtyp == PM_TULANI_ELADRIN || \
							 (ptr)->mtyp == PM_GAE_ELADRIN || \
							 (ptr)->mtyp == PM_BRIGHID_ELADRIN || \
							 (ptr)->mtyp == PM_UISCERRE_ELADRIN || \
							 (ptr)->mtyp == PM_CAILLEA_ELADRIN \
							)
#define is_archon(ptr)		((ptr)->mlet == S_LAW_ANGEL &&\
							 !((ptr)->mtyp == PM_COUATL ||\
							   (ptr)->mtyp == PM_ALEAX ||\
							   (ptr)->mtyp == PM_KI_RIN ||\
							   (ptr)->mtyp == PM_GIANT_EAGLE ||\
							   (ptr)->mtyp == PM_GOD ||\
							   (ptr)->mtyp == PM_DAMAGED_ARCADIAN_AVENGER ||\
							   (ptr)->mtyp == PM_ARCADIAN_AVENGER ||\
							   (ptr)->mtyp == PM_APOLLYON ||\
							   (ptr)->mtyp == PM_ANGEL))
#define is_auton(ptr)		(	(ptr)->mtyp == PM_MONOTON ||\
								(ptr)->mtyp == PM_DUTON ||\
								(ptr)->mtyp == PM_TRITON ||\
								(ptr)->mtyp == PM_QUATON ||\
								(ptr)->mtyp == PM_QUINON ||\
								(ptr)->mtyp == PM_AXUS\
							)
#define is_kamerel(ptr)		(	(ptr)->mtyp == PM_AMM_KAMEREL ||\
								(ptr)->mtyp == PM_HUDOR_KAMEREL ||\
								(ptr)->mtyp == PM_SHARAB_KAMEREL ||\
								(ptr)->mtyp == PM_ARA_KAMEREL\
							)
#define is_rilmani(ptr)		(	(ptr)->mtyp == PM_PLUMACH_RILMANI ||\
								(ptr)->mtyp == PM_FERRUMACH_RILMANI ||\
								(ptr)->mtyp == PM_CUPRILACH_RILMANI ||\
								(ptr)->mtyp == PM_STANNUMACH_RILMANI ||\
								(ptr)->mtyp == PM_ARGENACH_RILMANI ||\
								(ptr)->mtyp == PM_MERCURIAL_ESSENCE ||\
								(ptr)->mtyp == PM_BRIMSTONE_ESSENCE ||\
								(ptr)->mtyp == PM_HYDRARGYRUMACH_RILMANI ||\
								(ptr)->mtyp == PM_CENTER_OF_ALL ||\
								(ptr)->mtyp == PM_ADAMACH ||\
								(ptr)->mtyp == PM_AURUMACH_RILMANI\
							)
#define is_deva(ptr)		((ptr)->mlet == S_NEU_ANGEL)
#define is_divider(ptr)		( (ptr)->mtyp == PM_BLACK_PUDDING\
							  || (ptr)->mtyp == PM_BROWN_PUDDING\
							  || (ptr)->mtyp == PM_DARKNESS_GIVEN_HUNGER\
							  || (ptr)->mtyp == PM_GREMLIN\
							  || (ptr)->mtyp == PM_DUNGEON_FERN_SPORE\
							  || (ptr)->mtyp == PM_DUNGEON_FERN_SPROUT\
							  || (ptr)->mtyp == PM_BURNING_FERN_SPORE\
							  || (ptr)->mtyp == PM_BURNING_FERN_SPROUT\
							  || (ptr)->mtyp == PM_SWAMP_FERN_SPORE\
							  || (ptr)->mtyp == PM_SWAMP_FERN_SPROUT\
							  || (ptr)->mtyp == PM_RAZORVINE\
							)
#define is_horror(ptr)		(  (ptr)->mtyp == PM_SHAMBLING_HORROR\
							|| (ptr)->mtyp == PM_STUMBLING_HORROR\
							|| (ptr)->mtyp == PM_WANDERING_HORROR\
							|| (ptr)->mtyp == PM_NAMELESS_HORROR\
							)
#define is_chuul(ptr)		(  (ptr)->mtyp == PM_CHUUL \
							|| (ptr)->mtyp == PM_ELDER_CHUUL \
							|| (ptr)->mtyp == PM_BLIBDOOLPOOLP_S_MINDGRAVEN_CHAMPION \
							|| (ptr)->mtyp == PM_BLIBDOOLPOOLP__GRAVEN_INTO_FLESH \
							)
#define is_mercenary(ptr)	(((ptr)->mflagsg & MG_MERC) != 0L)
#define is_army_pm(pm)		(pm == PM_CAPTAIN || pm == PM_LIEUTENANT || pm == PM_SERGEANT || pm == PM_SOLDIER)
#define is_bardmon(ptr)		((ptr)->mtyp == PM_LILLEND || (ptr)->mtyp == PM_RHYMER || (ptr)->mtyp == PM_BARD)
#define is_male(ptr)		(((ptr)->mflagsb & MB_MALE) != 0L)
#define is_female(ptr)		(((ptr)->mflagsb & MB_FEMALE) != 0L)
#define is_neuter(ptr)		(((ptr)->mflagsb & MB_NEUTER) != 0L)
#define is_wanderer(ptr)	(((ptr)->mflagst & MT_WANDER) != 0L)
#define	is_goat_tentacle_mtyp(ptr)	((ptr)->mtyp == PM_DARK_YOUNG \
									|| (ptr)->mtyp == PM_MOTHERING_MASS \
									|| (ptr)->mtyp == PM_BLESSED \
									|| (ptr)->mtyp == PM_PARASITIC_WALL_HUGGER \
									|| (ptr)->mtyp == PM_SWIRLING_MIST \
									|| (ptr)->mtyp == PM_DUST_STORM \
									|| (ptr)->mtyp == PM_ICE_STORM \
									|| (ptr)->mtyp == PM_THUNDER_STORM \
									|| (ptr)->mtyp == PM_FIRE_STORM \
									|| (ptr)->mtyp == PM_MOUTH_OF_THE_GOAT)
#define	is_goat_tentacle_mon(mon)	(is_goat_tentacle_mtyp((mon)->data) || has_template(mon, MISTWEAVER))
#define	is_snake_bite_mtyp(ptr)	((ptr)->mtyp == PM_MEDUSA \
									|| (ptr)->mtyp == PM_ANCIENT_NAGA)
#define	is_snake_bite_mon(mon)	(is_snake_bite_mtyp((mon)->data) || has_template(mon, MOLY_TEMPLATE))
#define	is_tailslap_mtyp(ptr)	(is_true_adult_dragon(ptr) || (ptr)->mtyp == PM_UISCERRE_ELADRIN || (ptr)->mtyp == PM_DISENCHANTER || (ptr)->mtyp == PM_GRAY_DEVOURER)
#define	is_tailslap_mon(mon)	(is_tailslap_mtyp((mon)->data))

#define	is_vines_mon(mon)	((mon)->mtyp == PM_GAE_ELADRIN && (mon)->m_lev >= 20)
#define	is_star_blades_mtyp(ptr)	((ptr)->mtyp == PM_COURE_ELADRIN || (ptr)->mtyp == PM_FLAXEN_STARSHADOW || (ptr)->mtyp == PM_MOTE_OF_LIGHT || (ptr)->mtyp == PM_IASOIAN_ARCHON)
#define	is_star_blades_mon(mon)	(is_star_blades_mtyp((mon)->data) || ((mon)->mtyp == PM_PORO_AULON && (mon)->mhp<(mon)->mhpmax/2))
#define	is_storm_mtyp(ptr)	((ptr)->mtyp == PM_DAO_LAO_GUI_MONK)
#define	is_level_storm_mtyp(ptr)	((ptr)->mtyp == PM_CAILLEA_ELADRIN || (ptr)->mtyp == PM_MOONSHADOW\
							 || (ptr)->mtyp == PM_GHAELE_ELADRIN || (ptr)->mtyp == PM_LUMINOUS_CLOUD\
							 || (ptr)->mtyp == PM_PYROCLASTIC_VORTEX)
#define	is_storm_mon(mon)	((is_level_storm_mtyp((mon)->data) && (mon)->m_lev >= 20) || is_storm_mtyp((mon)->data))
#define	is_dancer(ptr)	((ptr)->mtyp == PM_PORO_AULON || (ptr)->mtyp == PM_SEYLL_AUZKOVYN || (ptr)->mtyp == PM_ANULO_DANCER || (ptr)->mtyp == PM_MYRKALFAR_MATRON)

#define goat_monster(ptr) (In_lost_cities(&u.uz) ? lost_cities_goat_monster(ptr) : always_goat_monster(ptr))
#define always_goat_monster(ptr) (\
									   (ptr)->mtyp == PM_SMALL_GOAT_SPAWN \
									|| (ptr)->mtyp == PM_GOAT_SPAWN \
									|| (ptr)->mtyp == PM_GIANT_GOAT_SPAWN \
									|| (ptr)->mtyp == PM_MIGO_WORKER \
									|| (ptr)->mtyp == PM_MIGO_SOLDIER \
									|| (ptr)->mtyp == PM_MIGO_PHILOSOPHER \
									|| (ptr)->mtyp == PM_MIGO_QUEEN \
									|| (ptr)->mtyp == PM_DARK_YOUNG \
									|| (ptr)->mtyp == PM_BLESSED \
									|| (ptr)->mtyp == PM_SWIRLING_MIST \
									|| (ptr)->mtyp == PM_DUST_STORM \
									|| (ptr)->mtyp == PM_ICE_STORM \
									|| (ptr)->mtyp == PM_THUNDER_STORM \
									|| (ptr)->mtyp == PM_FIRE_STORM \
									|| (ptr)->mtyp == PM_MOUTH_OF_THE_GOAT \
								  )

#define lost_cities_goat_monster(ptr) (\
									   always_goat_monster(ptr) \
									|| (ptr)->mtyp == PM_PLAINS_CENTAUR \
									|| (ptr)->mtyp == PM_FOREST_CENTAUR \
									|| (ptr)->mtyp == PM_MOUNTAIN_CENTAUR \
									|| (ptr)->mtyp == PM_QUICKLING \
									|| (ptr)->mtyp == PM_NAIAD \
									|| (ptr)->mtyp == PM_DRYAD \
									|| (ptr)->mtyp == PM_OREAD \
									|| (ptr)->mtyp == PM_YUKI_ONNA \
									|| (ptr)->mtyp == PM_DEMINYMPH \
									|| (ptr)->mtyp == PM_WHITE_UNICORN \
									|| (ptr)->mtyp == PM_GRAY_UNICORN \
									|| (ptr)->mtyp == PM_BLACK_UNICORN \
									|| (ptr)->mtyp == PM_NIGHTMARE \
								  )

#define always_yellow_monster(ptr) (\
									   (ptr)->mtyp == PM_BYAKHEE \
									|| (ptr)->mtyp == PM_COILING_BRAWN \
									|| (ptr)->mtyp == PM_FUNGAL_BRAIN \
									|| (ptr)->mtyp == PM_STRANGER \
									|| (ptr)->mtyp == PM_SUZERAIN \
									|| (ptr)->mtyp == PM_FLAXEN_STARSHADOW \
									|| (ptr)->mtyp == PM_FLAXEN_STAR_PHANTOM \
									|| (ptr)->mtyp == PM_PUPPET_EMPEROR_XELETH \
									|| (ptr)->mtyp == PM_PUPPET_EMPRESS_XEDALLI \
								  )

#define yellow_monster(mon) (\
							   always_yellow_monster(mon->data) \
							|| has_template(mon, YELLOW_TEMPLATE) \
							|| has_template(mon, DREAM_LEECH) \
							)

#define gates_in_help(ptr)	((is_demon((ptr)) || is_minion((ptr))) \
								&& !is_auton(ptr) \
								&& (ptr)->mtyp != PM_OONA \
								&& (ptr)->mtyp != PM_CHAOS \
								&& (ptr)->mtyp != PM_KARY__THE_FIEND_OF_FIRE \
								&& (ptr)->mtyp != PM_BALROG \
								&& (ptr)->mtyp != PM_DURIN_S_BANE \
								&& (ptr)->mtyp != PM_SUCCUBUS \
								&& (ptr)->mtyp != PM_INCUBUS \
								)

#define template_blocks_gate(magr)	(has_template(magr, ZOMBIFIED)\
									|| has_template(magr, SKELIFIED)\
									|| has_template(magr, CRYSTALFIED)\
									|| has_template(magr, TOMB_HERD)\
									|| has_template(magr, SLIME_REMNANT)\
								)
#define always_one_hand_mtyp(ptr)	(ptr->mtyp == PM_THRONE_ARCHON \
										|| ptr->mtyp == PM_OCCULTIST \
										|| ptr->mtyp == PM_LUNGORTHIN \
										|| ptr->mtyp == PM_BASTARD_OF_THE_BOREAL_VALLEY \
										|| ptr->mtyp == PM_GUG \
										|| ptr->mtyp == PM_Y_CULTIST_MATRON \
										|| ptr->mtyp == PM_Y_CULTIST_PATRON \
										|| ptr->mtyp == PM_DEMOGORGON \
									)
#define always_hostile(ptr)	(((ptr)->mflagst & MT_HOSTILE) != 0L)
#define always_hostile_mon(mon)	(always_hostile((mon)->data))
#define always_peaceful(ptr)	(((ptr)->mflagst & MT_PEACEFUL) != 0L)
#define race_hostile(ptr)	(((ptr)->mflagsa & urace.hatemask) != 0L)
#define race_peaceful(ptr)	(((ptr)->mflagsa & urace.lovemask) != 0L)
#define extra_nasty(ptr)	(((ptr)->mflagsg & MG_NASTY) != 0L)
#define strongmonst(ptr)	(((ptr)->mflagsb & MB_STRONG) != 0L)
#define can_breathe(ptr)	attacktype(ptr, AT_BREA)
#define taxes_sanity(ptr)	(((ptr)->mflagsg & MG_SANLOSS) != 0L)
#define yields_insight(ptr)	(((ptr)->mflagsg & MG_INSIGHT) != 0L)
#define is_render(mtyp)		(mtyp == PM_SECRET_WHISPERER || mtyp == PM_TRUTH_SEER || mtyp == PM_DREAM_EATER || mtyp == PM_VEIL_RENDER)
#define banish_kill(mtyp)	(is_render(mtyp))

#define mon_insane(mtmp)	(mtmp->mcrazed \
							 || mtmp->mberserk \
							 || mtmp->mdisrobe \
							 || mtmp->mdoubt \
							 || mtmp->msanctity \
							 || mtmp->mgluttony \
							 || mtmp->mfrigophobia \
							 || mtmp->mcannibal \
							 || mtmp->mrage \
							 || mtmp->margent \
							 || mtmp->msuicide \
							 || mtmp->mnudist \
							 || mtmp->mophidio \
							 || mtmp->marachno \
							 || mtmp->mentomo \
							 || mtmp->mthalasso \
							 || mtmp->mhelmintho \
							 || mtmp->mparanoid \
							 || mtmp->mtalons \
							 || mtmp->mdreams \
							 || mtmp->msciaphilia \
							 || mtmp->mforgetful \
							 || mtmp->mapostasy \
							 || mtmp->mtoobig \
							 || mtmp->mrotting \
							 || mtmp->mformication \
							 || mtmp->mscorpions)

#define has_sunflask(mtyp)	(mtyp == PM_UNDEAD_KNIGHT || mtyp == PM_WARRIOR_OF_SUNLIGHT\
							 || mtyp == PM_UNDEAD_MAIDEN || mtyp == PM_KNIGHT_OF_THE_PRINCESS_S_GUARD\
							 || mtyp == PM_BLUE_SENTINEL || mtyp == PM_DARKMOON_KNIGHT\
							 || mtyp == PM_UNDEAD_REBEL || mtyp == PM_PARDONER || mtyp == PM_OCCULTIST\
							 || mtyp == PM_CROW_WINGED_HALF_DRAGON || mtyp == PM_ADVENTURESS_ZARIA \
							)

#define triggers_ophidiophobia(mon)	(triggers_ophidiophobia_ptr((mon)->data) || has_template(mon, MOLY_TEMPLATE))

#define triggers_ophidiophobia_ptr(ptr)	(is_snake((ptr))\
		|| (ptr)->mlet == S_NAGA\
		|| (ptr)->mtyp == PM_COUATL\
		|| (ptr)->mtyp == PM_LILLEND\
		|| (ptr)->mtyp == PM_MEDUSA\
		|| (ptr)->mtyp == PM_MARILITH\
		|| (ptr)->mtyp == PM_MAMMON\
		|| (ptr)->mtyp == PM_SHAKTARI\
		|| (ptr)->mtyp == PM_DEMOGORGON\
		|| (ptr)->mtyp == PM_GIANT_EEL\
		|| (ptr)->mtyp == PM_ELECTRIC_EEL\
		|| (ptr)->mtyp == PM_KRAKEN\
		|| (ptr)->mtyp == PM_SALAMANDER\
		|| (ptr)->mtyp == PM_KARY__THE_FIEND_OF_FIRE\
		|| (ptr)->mtyp == PM_CATHEZAR)

#define triggers_entomophobia(ptr)	(is_insectoid(ptr) || is_arachnid(ptr))

#define triggers_arachnophobia(ptr)	(is_spider(ptr)\
		|| (ptr)->mtyp == PM_SPROW\
		|| (ptr)->mtyp == PM_DRIDER\
		|| (ptr)->mtyp == PM_PRIESTESS_OF_GHAUNADAUR\
		|| (ptr)->mtyp == PM_AVATAR_OF_LOLTH)

#define triggers_helminthophobia(mon)	((mon)->data->mlet == S_WORM || mon_attacktype((mon), AT_TENT))

#define allow_wield(ptr)	(ptr->mtyp == PM_SHOGGOTH \
							|| ptr->mtyp == PM_PRIEST_OF_GHAUNADAUR\
							|| ptr->mtyp == PM_MOTHERING_MASS\
							|| ptr->mtyp == PM_ANCIENT_NUPPERIBO\
							|| ptr->mtyp == PM_KOBOLD_SHAMAN\
							|| ptr->mtyp == PM_ORC_SHAMAN\
							|| ptr->mtyp == PM_MORDOR_SHAMAN\
							|| ptr->mtyp == PM_ZETA_METROID\
							|| ptr->mtyp == PM_MINOTAUR_PRIESTESS\
							|| ptr->mtyp == PM_BLASPHEMOUS_LURKER\
							|| ptr->mtyp == PM_DREAD_SERAPH\
							|| ptr->mtyp == PM_MAD_SEER\
							|| ptr->mtyp == PM_ORACLE\
							|| ptr->mtyp == PM_PRIEST_OF_AN_UNKNOWN_GOD\
							|| ptr->mtyp == PM_PRIEST_OF_IB\
							|| ptr->mtyp == PM_PALE_NIGHT\
							|| ptr->mtyp == PM_SARA__THE_LAST_ORACLE\
							|| ptr->mtyp == PM_STRANGE_CORPSE\
							|| ptr->mtyp == PM_NIMUNE\
							|| ptr->mtyp == PM_ELDER_BRAIN\
							|| ptr->mtyp == PM_KRAKEN__THE_FIEND_OF_WATER\
							|| ptr->mtyp == PM_CHAIN_GOLEM\
							|| ptr->mtyp == PM_GOOD_NEIGHBOR\
							)

#define allow_shield(ptr)	(ptr->mtyp == PM_SCORPION\
							|| ptr->mtyp == PM_YURIAN\
							|| ptr->mtyp == PM_ZETA_METROID\
							|| ptr->mtyp == PM_DEMOGORGON\
							|| ptr->mtyp == PM_CHAIN_GOLEM\
							|| ptr->mtyp == PM_SCORPIUS\
							|| ptr->mtyp == PM_KRAKEN__THE_FIEND_OF_WATER\
							)

#define could_twoweap(ptr)	attacktype(ptr, AT_XWEP)
#define could_twoweap_mon(mon)	mon_attacktype(mon, AT_XWEP)
// define cantweararm(ptr)	(breakarm(ptr) || sliparm(ptr))
#define arm_size_fits(ptr,obj)	(Is_dragon_scales(obj) || \
								 obj->objsize == (ptr)->msize || \
								 (is_elven_armor(obj) && abs(obj->objsize - (ptr)->msize) <= 1))
#define arm_match(ptr,obj)	(Is_dragon_scales(obj) || obj->otyp == WAISTCLOTH ||\
							((obj->otyp == ELVEN_TOGA || obj->otyp == NOBLE_S_DRESS || obj->otyp == GENTLEWOMAN_S_DRESS) && upper_body_match(ptr,obj)) ||\
							(ptr->mtyp == PM_BLIBDOOLPOOLP_S_MINDGRAVEN_CHAMPION && upper_body_match(ptr,obj)) ||\
							(full_body_match(ptr,obj)))
#define full_body_match(ptr,obj)	(((ptr->mflagsb&MB_BODYTYPEMASK) != 0) && \
		((ptr->mflagsb&MB_BODYTYPEMASK) == (obj->bodytypeflag&MB_BODYTYPEMASK)))
#define boots_size_fits(ptr,obj)	(ptr->mtyp == PM_BLIBDOOLPOOLP_S_MINDGRAVEN_CHAMPION ? \
								 (ptr->msize+1 == obj->objsize) : \
								 (ptr->msize == obj->objsize))
#define can_wear_gloves(ptr)	(!nogloves(ptr) && !nohands(ptr))
#define can_wear_amulet(ptr)	(has_head(ptr) || (ptr->mflagsb&MB_CAN_AMULET))
#define can_wear_blindf(ptr)	(has_head(ptr))
#define can_wear_boots(ptr)	((humanoid(ptr) || humanoid_feet(ptr)) && !nofeet(ptr) && !nolimbs(ptr))
#define shirt_match(ptr,obj)	((obj->otyp != BODYGLOVE && upper_body_match(ptr,obj)) || \
								full_body_match(ptr,obj))
#define upper_body_match(ptr,obj)	(((ptr->mflagsb&MB_HUMANOID) && (obj->bodytypeflag&MB_HUMANOID)) || \
		(((ptr->mflagsb&MB_BODYTYPEMASK) != 0) && ((ptr->mflagsb&MB_BODYTYPEMASK) == (obj->bodytypeflag&MB_BODYTYPEMASK))))
#define helm_match(ptr,obj)	((!has_horns(ptr) || obj->otyp == find_gcirclet() || is_flimsy(obj)) && !nohat(ptr) && \
						(is_hat(obj) || (has_head(ptr) && (ptr->mflagsb&MB_HEADMODIMASK) == (obj->bodytypeflag&MB_HEADMODIMASK))))
/*Note: No-modifier helms are "normal"*/
#define helm_size_fits(ptr,obj)	(obj->objsize == ptr->msize || (is_hat(obj) && obj->objsize <= ptr->msize))

#define hates_holy_mon(mon)	((mon) == &youmonst ? hates_holy(youracedata) : hates_holy((mon)->data))
#define hates_holy(ptr)		(is_demon(ptr) || is_undead(ptr) || (((ptr)->mflagsg&MG_HATESHOLY) != 0))
#define hates_unholy(ptr)	((ptr->mflagsg&MG_HATESUNHOLY) != 0)
#define hates_unholy_mon(mon)	((mon) == &youmonst ? hates_unholy(youracedata) : hates_unholy((mon)->data))
#define hates_unblessed(ptr)	((ptr->mflagsg&MG_HATESUNBLESSED) != 0)
#define hates_unblessed_mon(mon)	((mon) == &youmonst ? hates_unblessed(youracedata) : hates_unblessed((mon)->data))
#define hates_silver(ptr)	((ptr->mflagsg&MG_HATESSILVER) != 0)
#define hates_iron(ptr)		((ptr->mflagsg&MG_HATESIRON) != 0)
#define hates_lawful(ptr)		(is_chaotic(ptr) || (ptr)->mtyp == PM_UVUUDAUM)
#define hates_lawful_mon(mon)	((mon) == &youmonst ? u.ualign.type == A_CHAOTIC : (HAS_EPRI(mon) ? EPRI(mon)->shralign == A_CHAOTIC :\
				  HAS_EMIN(mon) ? EMIN(mon)->min_align == A_CHAOTIC :\
				  hates_lawful((mon)->data)))
#define hates_chaos(ptr)		(is_lawful(ptr) || (ptr)->mtyp == PM_UVUUDAUM)
#define hates_chaos_mon(mon)	((mon) == &youmonst ? u.ualign.type == A_LAWFUL : (HAS_EPRI(mon) ? EPRI(mon)->shralign == A_LAWFUL :\
				  HAS_EMIN(mon) ? EMIN(mon)->min_align == A_LAWFUL :\
				  hates_chaos((mon)->data)))

#define sflm_target(mon) (!is_rider((mon)->data) && (is_angel((mon)->data) || ((mon) == &youmonst ? Displaced : mon_resistance(mon, DISPLACED)) || is_shapechanger((mon)->data) || is_chaotic_mon(mon)))
#define sflm_target_data(ptr) (!is_rider(ptr) && (is_angel(ptr) || ((ptr)->mflagsg&MG_DISPLACEMENT) || is_shapechanger(ptr) || is_chaotic(ptr)))

#define melee_polearms(ptr)	((ptr)->mtyp == PM_VROCK ||\
							 (ptr)->mtyp == PM_MEPHISTOPHELES ||\
							 (ptr)->mtyp == PM_BAPHOMET \
							)
#define throws_rocks(ptr)	(((ptr)->mflagst & MT_ROCKTHROW) != 0L)
#define type_is_pname(ptr)	(((ptr)->mflagsg & MG_PNAME) != 0L)
#define is_thief(ptr)		( dmgtype(ptr, AD_SGLD)  || dmgtype(ptr, AD_SITM) || dmgtype(ptr, AD_SEDU) )
#define is_magical(ptr)		( attacktype(ptr, AT_MMGC) || attacktype(ptr, AT_MAGC) )
#define nospellcooldowns(ptr)	(((ptr)->mflagsg & MG_NOSPELLCOOLDOWN) != 0L)
#define nospellcooldowns_mon(mtmp)	(nospellcooldowns((mtmp)->data) || (is_alabaster_mummy((mtmp)->data) && (mtmp)->mvar_syllable == SYLLABLE_OF_THOUGHT__NAEN))
#define is_lord(ptr)		(((ptr)->mflagsg & MG_LORD) != 0L)
#define is_prince(ptr)		(((ptr)->mflagsg & MG_PRINCE) != 0L)
#define is_normal_demon(ptr)		(is_demon(ptr) && \
				 (((ptr)->mflagsg & (MG_LORD|MG_PRINCE)) == 0L))
#define is_dlord(ptr)		(is_demon(ptr) && is_lord(ptr))
#define is_dprince(ptr)		(is_demon(ptr) && is_prince(ptr))
#define is_dnoble(ptr)		(is_demon(ptr) && (is_lord(ptr) || is_prince(ptr)))
#define is_minion(ptr)		((ptr)->mflagsa & MA_MINION)
#define likes_gold(ptr)		(((ptr)->mflagst & MT_GREEDY) != 0L)
#define likes_gems(ptr)		(((ptr)->mflagst & MT_JEWELS) != 0L)
#define likes_objs(ptr)		(((ptr)->mflagst & MT_COLLECT) != 0L || \
				 (is_armed(ptr) && !mindless(ptr)))
#define likes_magic(ptr)	(((ptr)->mflagst & MT_MAGIC) != 0L)
#define webmaker(ptr)		((ptr)->mtyp == PM_CAVE_SPIDER || \
				 (ptr)->mtyp == PM_GIANT_SPIDER || (ptr)->mtyp == PM_PHASE_SPIDER || \
				 (ptr)->mtyp == PM_MIRKWOOD_SPIDER || (ptr)->mtyp == PM_MIRKWOOD_ELDER || \
				 (ptr)->mtyp == PM_SPROW || (ptr)->mtyp == PM_DRIDER || (ptr)->mtyp == PM_ALIDER || \
				 (ptr)->mtyp == PM_MENDICANT_SPROW || (ptr)->mtyp == PM_MENDICANT_DRIDER || \
				 (ptr)->mtyp == PM_EDDERKOP || \
				 (ptr)->mtyp == PM_AVATAR_OF_LOLTH || (ptr)->mtyp == PM_DROW_MUMMY)
#define is_unicorn(ptr)		((ptr)->mlet == S_UNICORN && likes_gems(ptr))
#define is_longworm(ptr)	(\
				 ((ptr)->mtyp == PM_LONG_WORM) || \
				 ((ptr)->mtyp == PM_LONG_WORM_TAIL) || \
				 ((ptr)->mtyp == PM_HUNTING_HORROR) || \
				 ((ptr)->mtyp == PM_HUNTING_HORROR_TAIL))
#define is_shapechanger(ptr)	((ptr)->mtyp == PM_CHAMELEON\
							  || (ptr)->mtyp == PM_DOPPELGANGER\
							  || (ptr)->mtyp == PM_SANDESTIN\
							  || (ptr)->mtyp == PM_DREAM_QUASIELEMENTAL\
							  || is_were(ptr)\
							  || (ptr)->mlet == S_MIMIC\
								 )
#define wants_bell(ptr)	((ptr->mflagst & MT_WANTSBELL))
#define wants_book(ptr)	((ptr->mflagst & MT_WANTSBOOK))
#define wants_cand(ptr)	((ptr->mflagst & MT_WANTSCAND))
#define wants_qart(ptr)	((ptr->mflagst & MT_WANTSARTI))
#define wants_amul(ptr)	((ptr->mflagst & MT_COVETOUS))
#define is_covetous(ptr)	((ptr->mflagst & MT_COVETOUS))

#define quest_faction(mon)	((mon)->mfaction == QUEST_FACTION || (Race_if(PM_DROW) && (mon)->mfaction == u.uhouse))

#define normalvision(ptr)	((ptr->mflagsv & MV_NORMAL))
#define darksight(ptr)		((ptr->mflagsv & MV_DARKSIGHT))
#define catsight(ptr)		((ptr->mflagsv & MV_CATSIGHT))
#define lowlightsight2(ptr)	((ptr->mflagsv & MV_LOWLIGHT2))
#define lowlightsight3(ptr)	((ptr->mflagsv & MV_LOWLIGHT3))
#define echolocation(ptr)	((ptr->mflagsv & MV_ECHOLOCATE))
#define extramission(ptr)	((ptr->mflagsv & MV_EXTRAMISSION))
#define rlyehiansight(ptr)	((ptr->mflagsv & MV_RLYEHIAN))

#define infravision(ptr)	((ptr->mflagsv & MV_INFRAVISION))
#define infravisible(ptr)	((ptr->mflagsg & MG_INFRAVISIBLE))
#define infravisible_mon(mon)	(infravisible((mon)->data))
#define bloodsense(ptr)		((ptr->mflagsv & MV_BLOODSENSE))
#define lifesense(ptr)		((ptr->mflagsv & MV_LIFESENSE))
#define earthsense(ptr)		((ptr->mflagsv & MV_EARTHSENSE))
#define senseall(ptr)		((ptr->mflagsv & MV_DETECTION))
#define omnisense(ptr)		((ptr->mflagsv & MV_OMNI))

#define can_betray(ptr)		((ptr->mflagst & MT_TRAITOR))
#define opaque(ptr)	(((ptr)->mflagsg & MG_OPAQUE))
#define mteleport(ptr)	(((ptr)->mflagsm & MM_TENGTPORT))
#define is_mplayer(ptr)		(((ptr)->mtyp >= PM_ARCHEOLOGIST) && \
				 ((ptr)->mtyp <= PM_WIZARD))
#define is_deadly(ptr)		((ptr)->mflagsg & MG_DEADLY)
#define is_rider(ptr)		((ptr)->mflagsg & MG_RIDER)
#define rider_hp(ptr)		((ptr)->mflagsg & MG_RIDER_HP)
#define is_placeholder(ptr)	((ptr)->mtyp == PM_ORC || \
				 (ptr)->mtyp == PM_GIANT || \
				 (ptr)->mtyp == PM_ELF || \
				 (ptr)->mtyp == PM_HUMAN)

/* return TRUE if the monster tends to revive */
#define is_reviver(ptr)		(is_rider(ptr) || (ptr)->mlet == S_TROLL || (ptr)->mlet == S_FUNGUS)

#define is_demihuman(ptr)	((ptr->mflagsa & MA_DEMIHUMAN))

/* this returns the light's range, or 0 if none; if we add more light emitting
   monsters, we'll likely have to add a new light range field to mons[] 
   KEEP IN SYNC with MAX_RADIUS, circle_data, and circle_start[].
   Maximum allowable lightsource radius is currently 10 (30 after 3x lowlight modifier) */
#define emits_light(ptr)	(((ptr)->mlet == S_LIGHT || \
				  (ptr)->mtyp == PM_BRIGHT_WALKER || \
				  (ptr)->mtyp == PM_FLAMING_SPHERE || \
				  (ptr)->mtyp == PM_SHOCKING_SPHERE || \
				  (ptr)->mtyp == PM_PARASITIZED_DOLL || \
				  (ptr)->mtyp == PM_MOTE_OF_LIGHT || \
				  (ptr)->mtyp == PM_BALL_OF_LIGHT || \
				  (ptr)->mtyp == PM_BLOODY_SUNSET || \
				  (ptr)->mtyp == PM_BALL_OF_GOSSAMER_SUNLIGHT || \
				  (ptr)->mtyp == PM_LUMINOUS_CLOUD || \
				  (ptr)->mtyp == PM_HOOLOOVOO || \
				  (ptr)->mtyp == PM_LIGHTNING_PARAELEMENTAL || \
				  (ptr)->mtyp == PM_FALLEN_ANGEL || \
				  (ptr)->mtyp == PM_ANCIENT_OF_THOUGHT || \
				  (ptr)->mtyp == PM_DARK_WORM || \
				  (ptr)->mtyp == PM_FIRE_VORTEX) ? 1 : \
				 ((ptr)->mtyp == PM_FIRE_ELEMENTAL ||\
				  (ptr)->mtyp == PM_FLAMING_ORB || \
				  (ptr)->mtyp == PM_CANDLE_TREE || \
				  (ptr)->mtyp == PM_PARASITIZED_KNIGHT || \
				  (ptr)->mtyp == PM_DANCING_FLAME ||\
				  (ptr)->mtyp == PM_COTERIE_OF_MOTES ||\
				  (ptr)->mtyp == PM_BALL_OF_RADIANCE) ? 2 : \
				 ((ptr)->mtyp == PM_THRONE_ARCHON ||\
				  (ptr)->mtyp == PM_BEAUTEOUS_ONE ||\
				  (ptr)->mtyp == PM_DAO_LAO_GUI_MONK ||\
				 (ptr)->mtyp == PM_ASPECT_OF_THE_SILENCE) ? 3 : \
				 ((ptr)->mtyp == PM_BLESSED) ? 4 : \
				 ((ptr)->mtyp == PM_LIGHT_ARCHON|| \
				  (ptr)->mtyp == PM_GOD ||\
				  (ptr)->mtyp == PM_LUCIFER) ? 7 : \
				 ((ptr)->mtyp == PM_EDDERKOP) ? 8 : \
				 ((ptr)->mtyp == PM_SURYA_DEVA) ? 9 : \
				 0)
#define emits_light_mon(mon) (has_template(mon, ILLUMINATED) ? \
							 max(3, emits_light((mon)->data)) : \
							 emits_light((mon)->data))
#define Is_darklight_monster(ptr)	((ptr)->mtyp == PM_EDDERKOP\
					|| (ptr)->mtyp == PM_DARK_WORM\
					|| (ptr)->mtyp == PM_ASPECT_OF_THE_SILENCE\
					|| (ptr)->mtyp == PM_DAO_LAO_GUI_MONK\
					)
/*	[note: the light ranges above were reduced to 1 for performance...] */
#define likes_lava(ptr)		( \
			(ptr)->mtyp == PM_FIRE_ELEMENTAL || \
			(ptr)->mtyp == PM_SALAMANDER \
			)

#define pm_invisible(ptr) ( \
			(ptr)->mtyp == PM_STALKER || \
			(ptr)->mtyp == PM_BLACK_LIGHT ||\
			(ptr)->mtyp == PM_PHANTOM_FUNGUS ||\
			(ptr)->mtyp == PM_CENTER_OF_ALL ||\
			(ptr)->mtyp == PM_DARKNESS_GIVEN_HUNGER ||\
			(ptr)->mtyp == PM_ANCIENT_OF_DEATH\
			)

/* could probably add more */
#define likes_fire(ptr)		( \
			(ptr)->mtyp == PM_FIRE_VORTEX || \
			(ptr)->mtyp == PM_FLAMING_SPHERE || \
			likes_lava(ptr))

#define touch_petrifies(ptr)	((ptr)->mtyp == PM_COCKATRICE || \
				 (ptr)->mtyp == PM_CHICKATRICE)

#define mad_no_armor(mon)		((mon)->mdisrobe || (mon)->mnudist)

#define is_weeping(ptr)		((ptr)->mtyp == PM_WEEPING_ANGEL)

#define is_alienist(ptr)		(is_mind_flayer(ptr) || \
								 (ptr)->mlet == S_UMBER ||\
								 (ptr)->mtyp == PM_DROW_ALIENIST ||\
								 (ptr)->mtyp == PM_DARUTH_XAXOX ||\
								 (ptr)->mtyp == PM_EMBRACED_DROWESS\
								)
#define has_mind_blast_mon(mon)	((has_mind_blast((mon)->data) \
				 || has_template(mon, DREAM_LEECH) \
				) && !((mon)->mtyp == PM_MAD_SEER && (mon)->mspec_used)\
				  && ((mon)->mnotlaugh)\
				  && !((mon)->mcan)\
				)
#define has_mind_blast(ptr)	(is_mind_flayer(ptr) \
				 || (ptr)->mtyp == PM_BRAIN_GOLEM \
				 || (ptr)->mtyp == PM_SEMBLANCE \
				 || (ptr)->mtyp == PM_FUNGAL_BRAIN \
				 || (ptr)->mtyp == PM_LADY_CONSTANCE \
				 || (ptr)->mtyp == PM_MADMAN \
				 || (ptr)->mtyp == PM_MADWOMAN \
				 || (ptr)->mtyp == PM_MAD_SEER \
				 || (ptr)->mtyp == PM_CLAIRVOYANT_CHANGED \
				)

#define insightful(ptr)	(yields_insight(ptr) \
				 || is_great_old_one(ptr) \
				 || is_primordial(ptr) \
				 || has_mind_blast(ptr) \
				 || (ptr)->mtyp == PM_JRT_NETJER \
				)

#define is_mind_flayer(ptr)	((ptr)->mtyp == PM_MIND_FLAYER || \
				 (ptr)->mtyp == PM_MASTER_MIND_FLAYER || \
				 (ptr)->mtyp == PM_STAR_SPAWN || \
				 (ptr)->mtyp == PM_MAD_GRAZI || \
				 (ptr)->mtyp == PM_PARASITIZED_ANDROID || \
				 (ptr)->mtyp == PM_PARASITIZED_GYNOID || \
				 (ptr)->mtyp == PM_PARASITIC_MIND_FLAYER || \
				 (ptr)->mtyp == PM_PARASITIC_MASTER_MIND_FLAYER || \
				 (ptr)->mtyp == PM_ALHOON || \
				 (ptr)->mtyp == PM_ELDER_BRAIN || \
				 (ptr)->mtyp == PM_LUGRIBOSSK || \
				 (ptr)->mtyp == PM_MAANZECORIAN || \
				 (ptr)->mtyp == PM_GREAT_CTHULHU \
				)

#define is_android(ptr)	((ptr)->mtyp == PM_ANDROID || \
				 (ptr)->mtyp == PM_GYNOID || \
				 (ptr)->mtyp == PM_OPERATOR || \
				 (ptr)->mtyp == PM_COMMANDER || \
				 (ptr)->mtyp == PM_MUMMIFIED_ANDROID || \
				 (ptr)->mtyp == PM_MUMMIFIED_GYNOID || \
				 (ptr)->mtyp == PM_FLAYED_ANDROID || \
				 (ptr)->mtyp == PM_FLAYED_GYNOID || \
				 (ptr)->mtyp == PM_CRUCIFIED_ANDROID || \
				 (ptr)->mtyp == PM_CRUCIFIED_GYNOID || \
				 (ptr)->mtyp == PM_PARASITIZED_ANDROID || \
				 (ptr)->mtyp == PM_PARASITIZED_GYNOID || \
				 (ptr)->mtyp == PM_PARASITIZED_OPERATOR || \
				 (ptr)->mtyp == PM_PARASITIZED_COMMANDER \
				)

#define free_android(ptr)	((ptr)->mtyp == PM_ANDROID || \
				 (ptr)->mtyp == PM_GYNOID || \
				 (ptr)->mtyp == PM_OPERATOR || \
				 (ptr)->mtyp == PM_COMMANDER \
				)

#define is_dollable(ptr)	((ptr)->mtyp == PM_ANDROID || \
				 (ptr)->mtyp == PM_GYNOID || \
				 (ptr)->mtyp == PM_OPERATOR || \
				 (ptr)->mtyp == PM_COMMANDER || \
				 (ptr)->mtyp == PM_LIVING_DOLL \
				)

#define stuck_in_time(mtmp) 		(mtmp->mtyp != PM_EDDERKOP\
		&& mtmp->mtyp != PM_EMBRACED_DROWESS\
		&& mtmp->mtyp != PM_PARASITIZED_EMBRACED_ALIDER\
		&& !is_mind_flayer(mtmp)\
		&& !has_template(mtmp, M_BLACK_WEB)\
		&& !has_template(mtmp, M_GREAT_WEB)\
		&& !has_template(mtmp, PSEUDONATURAL)\
		&& !has_template(mtmp, FRACTURED)\
		&& !is_naturally_unalive(mtmp->data)\
		)

#define is_changed_mtyp(mtyp)	(mtyp == PM_CHANGED || mtyp == PM_WARRIOR_CHANGED || mtyp == PM_TWITCHING_FOUR_ARMED_CHANGED || mtyp == PM_CLAIRVOYANT_CHANGED)

#define nonliving(ptr)	(is_unalive(ptr) || is_undead(ptr) || \
				 (ptr)->mtyp == PM_MANES \
				)

#define is_unalive(ptr)		(on_level(&valley_level, &u.uz) || is_naturally_unalive(ptr))

#define is_naturally_unalive(ptr)		(((ptr)->mflagsa & MA_UNLIVING))

#define is_indigestible(ptr)	(((ptr)->mflagsb&MB_INDIGESTIBLE) != 0)

// #define is_indigestible(ptr)	((ptr)->mtyp == PM_DANCING_BLADE ||\
								 // (ptr)->mtyp == PM_EARTH_ELEMENTAL ||\
								 // (ptr)->mtyp == PM_TERRACOTTA_SOLDIER ||\
								 // (ptr)->mtyp == PM_CLOCKWORK_SOLDIER ||\
								 // (ptr)->mtyp == PM_CLOCKWORK_DWARF ||\
								 // (ptr)->mtyp == PM_CLOCKWORK_FACTORY ||\
								 // (ptr)->mtyp == PM_GOLDEN_HEART ||\
								 // (ptr)->mtyp == PM_JUGGERNAUT ||\
								 // (ptr)->mtyp == PM_ID_JUGGERNAUT ||\
								 // (ptr)->mtyp == PM_SCRAP_TITAN ||\
								 // (ptr)->mtyp == PM_HELLFIRE_COLOSSUS ||\
								 // (ptr)->mtyp == PM_HELLFIRE_ORB ||\
								 // (ptr)->mtyp == PM_CLOCKWORK_AUTOMATON ||\
								 // (ptr)->mtyp == PM_COLOSSAL_CLOCKWORK_WAR_MACHINE ||\
								 // (ptr)->mtyp == PM_MALKUTH_SEPHIRAH ||\
								 // (ptr)->mtyp == PM_YESOD_SEPHIRAH ||\
								 // (ptr)->mtyp == PM_DAAT_SEPHIRAH ||\
								 // (ptr)->mtyp == PM_HOD_SEPHIRAH ||\
								 // (ptr)->mtyp == PM_NETZACH_SEPHIRAH ||\
								 // (ptr)->mtyp == PM_GEVURAH_SEPHIRAH ||\
								 // (ptr)->mtyp == PM_BINAH_SEPHIRAH ||\
								 // (ptr)->mtyp == PM_CHOKHMAH_SEPHIRAH ||\
								 // (ptr)->mtyp == PM_HALF_STONE_DRAGON ||\
								 // (ptr)->mtyp == PM_HOOLOOVOO ||\
								 // (ptr)->mtyp == PM_GOLD_GOLEM ||\
								 // (ptr)->mtyp == PM_CLAY_GOLEM ||\
								 // (ptr)->mtyp == PM_TREASURY_GOLEM ||\
								 // (ptr)->mtyp == PM_SEMBLANCE ||\
								 // (ptr)->mtyp == PM_STONE_GOLEM ||\
								 // (ptr)->mtyp == PM_GLASS_GOLEM ||\
								 // (ptr)->mtyp == PM_IRON_GOLEM ||\
								 // (ptr)->mtyp == PM_GREEN_STEEL_GOLEM ||\
								 // (ptr)->mtyp == PM_ARGENTUM_GOLEM ||\
								 // (ptr)->mtyp == PM_RETRIEVER ||\
								 // (ptr)->mtyp == PM_LIVING_DOLL ||\
								 // (ptr)->mtyp == PM_ARA_KAMEREL ||\
								 // (ptr)->mtyp == PM_ANCIENT_OF_DEATH ||\
								 // (ptr)->mtyp == PM_PALE_NIGHT ||\
								 // (ptr)->mtyp == PM_BAALPHEGOR ||\
								 // (ptr)->mtyp == PM_ARCADIAN_AVENGER ||\
								 // (ptr)->mtyp == PM_DAMAGED_ARCADIAN_AVENGER ||\
								 // (ptr)->mtyp == PM_MUMMIFIED_ANDROID ||\
								 // (ptr)->mtyp == PM_MUMMIFIED_GYNOID ||\
								 // (ptr)->mtyp == PM_FLAYED_ANDROID ||\
								 // (ptr)->mtyp == PM_FLAYED_GYNOID ||\
								 // (ptr)->mtyp == PM_CRUCIFIED_ANDROID ||\
								 // (ptr)->mtyp == PM_CRUCIFIED_GYNOID ||\
								 // (ptr)->mtyp == PM_ANDROID ||\
								 // (ptr)->mtyp == PM_GYNOID ||\
								 // (ptr)->mtyp == PM_OPERATOR ||\
								 // (ptr)->mtyp == PM_COMMANDER ||\
								 // (ptr)->mtyp == PM_SENTINEL_OF_MITHARDIR ||\
								 // (ptr)->mtyp == PM_CHAIN_GOLEM ||\
								 // is_uvuudaum(ptr) ||\
								 // is_rilmani(ptr))

#define is_delouseable(ptr) ((ptr)->mtyp == PM_PARASITIZED_DOLL ||\
								   (ptr)->mtyp == PM_PARASITIZED_ANDROID ||\
								   (ptr)->mtyp == PM_PARASITIZED_GYNOID ||\
								   (ptr)->mtyp == PM_PARASITIZED_OPERATOR ||\
								   (ptr)->mtyp == PM_PARASITIZED_COMMANDER)

#define is_elemental(ptr)		( (ptr->mflagsa & MA_ELEMENTAL) )

#define mon_archeologist(mon)	((mon)->mtyp == PM_ARCHEOLOGIST || (mon)->mtyp == PM_LORD_CARNARVON || (mon)->mtyp == PM_STUDENT || \
								((mon)->mtyp == PM_HOD_SEPHIRAH && Role_if(PM_ARCHEOLOGIST)) || \
								 ((mon)->mtyp == PM_DEMINYMPH && (mon)->mvar_deminymph_role == PM_ARCHEOLOGIST))
#define mon_anachrononaut(mon)	((mon)->mtyp == PM_ANACHRONONAUT || (mon)->mtyp == PM_TROOPER || is_myrkalfr(mon) || \
								((mon)->mtyp == PM_HOD_SEPHIRAH && Role_if(PM_ANACHRONONAUT)) || \
								 ((mon)->mtyp == PM_DEMINYMPH && (mon)->mvar_deminymph_role == PM_ANACHRONONAUT))
#define mon_barbarian(mon)	((mon)->mtyp == PM_BARBARIAN || (mon)->mtyp == PM_HALF_DRAGON || (mon)->mtyp == PM_ORC_WARCHIEF || \
							 (mon)->mtyp == PM_CHIEFTAIN || (mon)->mtyp == PM_ORC_WARRIOR || \
							 ((mon)->mtyp == PM_HOD_SEPHIRAH && Role_if(PM_BARBARIAN)) || \
							 ((mon)->mtyp == PM_DEMINYMPH && (mon)->mvar_deminymph_role == PM_BARBARIAN))
#define mon_bard(mon)	((mon)->mtyp == PM_BARD || (mon)->mtyp == PM_PINDAR || (mon)->mtyp == PM_RHYMER || \
						 ((mon)->mtyp == PM_HOD_SEPHIRAH && Role_if(PM_BARD)) || \
						 ((mon)->mtyp == PM_DEMINYMPH && (mon)->mvar_deminymph_role == PM_BARD))
#define mon_caveman(mon)	((mon)->mtyp == PM_CAVEMAN || (mon)->mtyp == PM_CAVEWOMAN || (mon)->mtyp == PM_SHAMAN_KARNOV || \
							 ((mon)->mtyp == PM_HOD_SEPHIRAH && Role_if(PM_CAVEMAN)) || \
							 (mon)->mtyp == PM_NEANDERTHAL || ((mon)->mtyp == PM_DEMINYMPH && (mon)->mvar_deminymph_role == PM_CAVEMAN))
#define mon_convict(mon)	((mon)->mtyp == PM_CONVICT || (mon)->mtyp == PM_ROBERT_THE_LIFER || (mon)->mtyp == PM_INMATE || \
							 ((mon)->mtyp == PM_HOD_SEPHIRAH && Role_if(PM_CONVICT)) || \
							 ((mon)->mtyp == PM_DEMINYMPH && (mon)->mvar_deminymph_role == PM_CONVICT))
#define mon_undead_hunter(mon)	(((mon)->mtyp == PM_DEMINYMPH && (mon)->mvar_deminymph_role == PM_HUNTER))
#define mon_knight(mon)	((mon)->mtyp == PM_KNIGHT || (mon)->mtyp == PM_KING_ARTHUR || (mon)->mtyp == PM_SIR_GARLAND || \
						 (mon)->mtyp == PM_GARLAND || (mon)->mtyp == PM_CELEBORN || (mon)->mtyp == PM_DANTRAG || \
						 ((mon)->mtyp == PM_HOD_SEPHIRAH && Role_if(PM_KNIGHT)) || \
						 (mon)->mtyp == PM_PAGE || ((mon)->mtyp == PM_DEMINYMPH && (mon)->mvar_deminymph_role == PM_KNIGHT))
#define mon_turn_undead(mon)	((mon)->mtyp == PM_KNIGHT || (mon)->mtyp == PM_KING_ARTHUR || \
						 (mon)->mtyp == PM_BRIGHID_ELADRIN || \
						 (mon)->mtyp == PM_DRACAE_ELADRIN || \
						 (mon)->mtyp == PM_KUKER || (mon)->mtyp == PM_SHIELD_ARCHON || \
						 (mon)->mtyp == PM_MONADIC_DEVA || \
						 (mon)->mtyp == PM_ALRUNES || (mon)->mtyp == PM_HATEFUL_WHISPERS || \
						 (mon)->mtyp == PM_ELF_LORD || (mon)->mtyp == PM_ELVENKING || \
						 (mon)->mtyp == PM_CELEBORN || (mon)->mtyp == PM_DANTRAG || \
						 (mon)->mtyp == PM_ARIANNA || (mon)->mtyp == PM_BLIBDOOLPOOLP_S_MINDGRAVEN_CHAMPION || \
						 (mon)->mtyp == PM_REBEL_RINGLEADER || (mon)->mtyp == PM_RADIANT_PYRAMID || \
						 (mon)->mtyp == PM_SIR_ALJANOR || (mon)->mtyp == PM_ALLIANCE_VANGUARD || \
						 ((mon)->mtyp == PM_HOD_SEPHIRAH && Role_if(PM_KNIGHT)) || \
						 ((mon)->mtyp == PM_DEMINYMPH && (mon)->mvar_deminymph_role == PM_KNIGHT))
#define mon_healing_turn(mon)	((mon)->mtyp == PM_DRACAE_ELADRIN)
#define mon_monk(mon)	((mon)->mtyp == PM_MONK || (mon)->mtyp == PM_GRAND_MASTER || (mon)->mtyp == PM_MASTER_KAEN || (mon)->mtyp == PM_ABBOT || \
						 (mon)->mtyp == PM_XORN_MONK || (mon)->mtyp == PM_DAO_LAO_GUI_MONK || (mon)->mtyp == PM_ZHI_REN_MONK || \
						 ((mon)->mtyp == PM_HOD_SEPHIRAH && Role_if(PM_MONK)) || \
						 (mon)->mtyp == PM_XUENU_MONK || ((mon)->mtyp == PM_DEMINYMPH && (mon)->mvar_deminymph_role == PM_MONK))
#define mon_madman(mon)	((mon)->mtyp == PM_MADMAN || (mon)->mtyp == PM_MADWOMAN || (mon)->mtyp == PM_CASSILDA_THE_IRON_MAIDEN || \
						 (mon)->mtyp == PM_PATIENT || (mon)->mtyp == PM_CONTAMINATED_PATIENT || \
						 ((mon)->mtyp == PM_HOD_SEPHIRAH && Role_if(PM_MADMAN)) || \
						 ((mon)->mtyp == PM_DEMINYMPH && (mon)->mvar_deminymph_role == PM_MADMAN))
#define mon_pirate(mon)	((mon)->mtyp == PM_PIRATE || (mon)->mtyp == PM_MAYOR_CUMMERBUND || (mon)->mtyp == PM_BLACKBEARD_S_GHOST || \
						 ((mon)->mtyp == PM_HOD_SEPHIRAH && Role_if(PM_PIRATE)) || \
						 (mon)->mtyp == PM_PIRATE_BROTHER || ((mon)->mtyp == PM_DEMINYMPH && (mon)->mvar_deminymph_role == PM_PIRATE))
#define mon_priest(mon)	((mon)->mtyp == PM_PRIEST || (mon)->mtyp == PM_PRIESTESS ||\
						 (mon)->mtyp == PM_ECLAVDRA || (mon)->mtyp == PM_GALADRIEL || (mon)->mtyp == PM_MOTHER ||\
						 (mon)->mtyp == PM_DROW_NOVICE || (mon)->mtyp == PM_ARCH_PRIEST || (mon)->mtyp == PM_SEYLL_AUZKOVYN || \
						 (mon)->mtyp == PM_A_SALOM || (mon)->mtyp == PM_DROW_MATRON_MOTHER || (mon)->mtyp == PM_HIGH_ELF || \
						 (mon)->mtyp == PM_STJARNA_ALFR || (mon)->mtyp == PM_SISTER || (mon)->mtyp == PM_ACOLYTE || \
						 (mon)->mtyp == PM_PRIEST_OF_GHAUNADAUR || (mon)->mtyp == PM_DWARF_CLERIC || (mon)->mtyp == PM_DWARF_QUEEN || \
						 (mon)->mtyp == PM_ELVENKING || (mon)->mtyp == PM_ELVENQUEEN || (mon)->mtyp == PM_MILITANT_CLERIC || \
						 ((mon)->mtyp == PM_HOD_SEPHIRAH && Role_if(PM_PRIEST)) || \
						 ((mon)->mtyp == PM_DEMINYMPH && (mon)->mvar_deminymph_role == PM_PRIEST))
#define mon_ranger(mon)	((mon)->mtyp == PM_RANGER || (mon)->mtyp == PM_ORION || (mon)->mtyp == PM_HALF_ELF_RANGER || (mon)->mtyp == PM_HUNTER || \
						 ((mon)->mtyp == PM_HOD_SEPHIRAH && Role_if(PM_RANGER)) || \
						 ((mon)->mtyp == PM_DEMINYMPH && (mon)->mvar_deminymph_role == PM_RANGER))
#define mon_rogue(mon)	((mon)->mtyp == PM_ROGUE || (mon)->mtyp == PM_MASTER_OF_THIEVES || \
						 ((mon)->mtyp == PM_HOD_SEPHIRAH && Role_if(PM_ROGUE)) || \
						 ((mon)->mtyp == PM_DEMINYMPH && (mon)->mvar_deminymph_role == PM_ROGUE))
#define mon_samurai(mon)	((mon)->mtyp == PM_SAMURAI || (mon)->mtyp == PM_LORD_SATO || (mon)->mtyp == PM_ASHIKAGA_TAKAUJI || (mon)->mtyp == PM_ROSHI || \
							 ((mon)->mtyp == PM_HOD_SEPHIRAH && Role_if(PM_SAMURAI)) || \
							 ((mon)->mtyp == PM_DEMINYMPH && (mon)->mvar_deminymph_role == PM_SAMURAI))
#define mon_tourist(mon)	((mon)->mtyp == PM_TOURIST || (mon)->mtyp == PM_TWOFLOWER || \
							 ((mon)->mtyp == PM_HOD_SEPHIRAH && Role_if(PM_TOURIST)) || \
							 ((mon)->mtyp == PM_DEMINYMPH && (mon)->mvar_deminymph_role == PM_TOURIST))
#define mon_valkyrie(mon)	((mon)->mtyp == PM_VALKYRIE || (mon)->mtyp == PM_AWAKENED_VALKYRIE || (mon)->mtyp == PM_TRANSCENDENT_VALKYRIE || \
							 ((mon)->mtyp == PM_HOD_SEPHIRAH && Role_if(PM_VALKYRIE)) || \
							 (mon)->mtyp == PM_WARRIOR || ((mon)->mtyp == PM_DEMINYMPH && (mon)->mvar_deminymph_role == PM_VALKYRIE))
#define mon_wizard(mon)	((mon)->mtyp == PM_WIZARD || (mon)->mtyp == PM_WORM_THAT_WALKS || (mon)->mtyp == PM_INCANTIFIER || \
						 (mon)->mtyp == PM_NEFERET_THE_GREEN || (mon)->mtyp == PM_NIMUNE || (mon)->mtyp == PM_ADVENTURING_WIZARD || \
						 (mon)->mtyp == PM_HEDROW_MASTER_WIZARD || (mon)->mtyp == PM_GROMPH || (mon)->mtyp == PM_GUIDE || \
						 ((mon)->mtyp == PM_HOD_SEPHIRAH && Role_if(PM_WIZARD)) || \
						 (mon)->mtyp == PM_APPRENTICE || ((mon)->mtyp == PM_DEMINYMPH && (mon)->mvar_deminymph_role == PM_WIZARD))

#define is_vectored_mtyp(mtyp)	(is_random_vectored_mtyp(mtyp) || is_guided_vectored_mtyp(mtyp))

#define is_random_vectored_mtyp(mtyp)	((mtyp) == PM_CLOCKWORK_SOLDIER || (mtyp) == PM_CLOCKWORK_DWARF || \
				   (mtyp) == PM_FABERGE_SPHERE || (mtyp) == PM_FIREWORK_CART)

#define is_guided_vectored_mtyp(mtyp)	((mtyp) == PM_JUGGERNAUT || (mtyp) == PM_ID_JUGGERNAUT)

#define is_smith_mtyp(mtyp)	((mtyp) == PM_OONA || (mtyp) == PM_DRACAE_ELADRIN || (mtyp) == PM_GOBLIN_SMITH || (mtyp) == PM_DWARF_SMITH\
							 || (mtyp) == PM_MITHRIL_SMITH || (mtyp) == PM_SHADOWSMITH || (mtyp) == PM_HUMAN_SMITH)
#define is_smith_mon(mon)	(get_mx(mon, MX_ESMT))

#define likes_swamp(ptr)	((ptr)->mlet == S_PUDDING || \
				 (ptr)->mlet == S_FUNGUS || \
				 (ptr)->mtyp == PM_OCHRE_JELLY)
#define stationary_mon(mon)		(stationary((mon)->data) || \
	((mon)->mtyp == PM_ETHEREAL_FILCHER && (mon)->mcan) || \
	((mon) != &youmonst ? (which_armor((mon), W_ARMF) && which_armor((mon), W_ARMF)->otyp == SHACKLES && which_armor((mon), W_ARMF)->cursed) :\
	(uarmf && uarmf->otyp == SHACKLES && uarmf->cursed)))
#define stationary(ptr)		((ptr)->mflagsm & MM_STATIONARY)
#define sessile(ptr)		((ptr)->mmove == 0)

#define straitjacketed_mon(mon) (mon->entangled_oid || (which_armor(mon, W_ARM) && which_armor(mon, W_ARM)->otyp == STRAITJACKET && which_armor(mon, W_ARM)->cursed))
#define shackled_arms_mon(mon) (which_armor(mon, W_ARMG) && which_armor(mon, W_ARMG)->otyp == SHACKLES && which_armor(mon, W_ARMG)->cursed)
#define covered_face_mon(mon) ((which_armor(mon, W_ARMH) && FacelessHelm(which_armor(mon, W_ARMH))) || \
							   (which_armor(mon, W_ARMC) && FacelessCloak(which_armor(mon, W_ARMC))) \
							)

/* Used for conduct with corpses, tins, and digestion attacks */
/* G_NOCORPSE monsters might still be swallowed as a purple worm */
/* Maybe someday this could be in mflags... */
/** Shogooths are vegan :-/ **/
#define vegan(ptr)		(((ptr)->mlet == S_BLOB \
							&& (ptr)->mtyp != PM_BLOB_OF_PRESERVED_ORGANS\
							&& (ptr)->mtyp != PM_BLOOD_BLOATER\
							&& (ptr)->mtyp != PM_COILING_BRAWN\
						 ) || \
				 (ptr)->mlet == S_JELLY ||            \
				((ptr)->mlet == S_FUNGUS && 		  \
					!is_migo(ptr)) ||				  \
				 (ptr)->mlet == S_VORTEX ||           \
				 (ptr)->mlet == S_LIGHT ||            \
				 (ptr)->mlet == S_PLANT ||            \
				 is_fern_spore(ptr) ||                \
				 is_elemental_sphere(ptr) ||          \
				 is_gizmo(ptr) ||                     \
				 (is_clockwork(ptr) &&                \
					is_naturally_unalive(ptr)) ||     \
				((ptr)->mlet == S_ELEMENTAL &&        \
				 (ptr)->mtyp != PM_STALKER) ||        \
				((ptr)->mlet == S_GOLEM &&            \
				 (ptr)->mtyp != PM_FLESH_GOLEM &&     \
				 (ptr)->mtyp != PM_LEATHER_GOLEM) ||  \
				 (ptr)->mtyp == PM_WOOD_TROLL ||      \
				 noncorporeal(ptr))
#define is_burnable(ptr)	((ptr)->mlet == S_PLANT || \
							((ptr)->mlet == S_FUNGUS && !is_migo(ptr)) || \
							is_fern_spore(ptr) || \
							(ptr)->mtyp == PM_WOOD_TROLL)
#define vegetarian(ptr)		(vegan(ptr) || \
				((ptr)->mlet == S_PUDDING &&         \
				 (ptr)->mtyp != PM_BLACK_PUDDING && \
				 (ptr)->mtyp != PM_DARKNESS_GIVEN_HUNGER))

/* For vampires */
#define has_blood(ptr)		(!vegetarian(ptr) && \
				   (ptr)->mlet != S_GOLEM && \
				   (ptr)->mlet != S_KETER && \
				   (ptr)->mlet != S_MIMIC && \
				   (ptr)->mtyp != PM_WEEPING_ANGEL && \
				   (ptr)->mtyp != PM_GREAT_CTHULHU && \
				   (ptr)->mtyp != PM_STAR_SPAWN && \
				   !is_clockwork(ptr) && \
				   (!nonliving(ptr) || is_vampire(ptr)))
#define has_blood_mon(mon)	(has_blood((mon)->data))

/* Keep track of ferns, fern sprouts, fern spores, and other plants */

#define is_fern_sprout(ptr)	((ptr)->mtyp == PM_DUNGEON_FERN_SPROUT || \
				 (ptr)->mtyp == PM_SWAMP_FERN_SPROUT || \
				 (ptr)->mtyp == PM_BURNING_FERN_SPROUT)

#define is_fern_spore(ptr)	((ptr)->mtyp == PM_DUNGEON_FERN_SPORE || \
				 (ptr)->mtyp == PM_SWAMP_FERN_SPORE || \
				 (ptr)->mtyp == PM_BURNING_FERN_SPORE)

#define is_fern(ptr)		(is_fern_sprout(ptr) || \
				 (ptr)->mtyp == PM_DUNGEON_FERN || \
				 (ptr)->mtyp == PM_SWAMP_FERN || \
				 (ptr)->mtyp == PM_BURNING_FERN)

#define is_elemental_sphere(ptr)	((ptr)->mtyp == PM_FREEZING_SPHERE || \
				 (ptr)->mtyp == PM_FLAMING_SPHERE || \
				 (ptr)->mtyp == PM_SHOCKING_SPHERE || \
				 (ptr)->mtyp == PM_VEXING_ORB || \
				 (ptr)->mtyp == PM_FLAMING_ORB)

#define is_vegetation(ptr)	((ptr)->mlet == S_PLANT || is_fern(ptr))

#ifdef CONVICT
#define befriend_with_obj(ptr, obj) (((obj)->oclass == FOOD_CLASS ||\
				      (obj)->otyp == SHEAF_OF_HAY) && \
				      (is_domestic(ptr) || (is_rat(ptr) && Role_if(PM_CONVICT))))
#else
#define befriend_with_obj(ptr, obj) (((obj)->oclass == FOOD_CLASS || \
				      (obj)->otyp == SHEAF_OF_HAY) && \
				      is_domestic(ptr))
#endif

#define can_see_hurtnss_of_mon(mon) \
	((u.sealsActive&SEAL_MOTHER && !is_undead((mon)->data)) || \
	(Role_if(PM_HEALER) && (!nonliving((mon)->data) || has_blood_mon(mon))) || \
	(ublindf && ublindf->otyp == ANDROID_VISOR))

#define standardUnwardable(mtmp) (mtmp->isshk || mtmp->iswiz || is_rider(mtmp->data))

#define catWardInactive ( 	(mvitals[PM_KITTEN].mvflags & G_GENOD || mvitals[PM_KITTEN].died >= 120) && \
				(mvitals[PM_HOUSECAT].mvflags & G_GENOD || mvitals[PM_HOUSECAT].died >= 120) && \
				(mvitals[PM_LARGE_CAT].mvflags & G_GENOD || mvitals[PM_LARGE_CAT].died >= 120) \
			)

#define	wingWarded(data)	(data->mlet == S_COCKATRICE \
			|| data->mlet == S_RODENT \
			|| data->mlet == S_NAGA \
			|| data->mlet == S_SNAKE \
			|| data->mlet == S_LIZARD \
			|| data->mtyp == PM_TOVE \
			|| data->mtyp == PM_KRAKEN \
		)

#define	catWarded(data)	(is_bird(data) \
			|| is_bat(data) \
			|| data->mlet == S_RODENT \
			|| data->mlet == S_SNAKE \
			|| data->mlet == S_SPIDER \
			|| data->mlet == S_EEL \
			|| data->mlet == S_LIZARD \
			|| data->mtyp == PM_TOVE \
		)

#define	touWarded(data)	(data->mlet == S_DOG || data->mlet == S_FELINE)

#define	dreWarded(data)	(is_bird(data)\
			|| is_bat(data) \
			|| data->mlet == S_QUADRUPED \
			|| data->mlet == S_UNICORN \
		)

#define	veiWarded(data)	(data->mlet == S_EEL)

#define	thjWarded(data)	(data->mlet == S_LEPRECHAUN \
			|| data->mlet == S_NYMPH \
			|| is_thief(data) \
		)

#define	queenWarded(data)	(data->mlet == S_ANT \
			|| data->mlet == S_XAN \
			|| data->mlet == S_SPIDER \
			|| data->mlet == S_RUSTMONST \
		)

#define	eeyeWarded(data)	( \
			jsjk;sfdlksakslakldskkflskals \
		)

#define	hamWarded(data)	(is_auton(data) \
			|| data->mtyp == PM_FLOATING_EYE \
			|| data->mtyp == PM_BEHOLDER \
			|| data->mtyp == PM_APHANACTONAN_ASSESSOR \
			|| data->mtyp == PM_EYE_OF_DOOM \
			|| data->mtyp == PM_MANY_EYED_SEEKER \
			|| data->mtyp == PM_GROTESQUE_PEEPER \
			|| data->mtyp == PM_JRT_NETJER \
			|| data->mtyp == PM_YOCHLOL \
			|| data->mtyp == PM_TRUTH_SEER \
			|| data->mtyp == PM_DREAM_EATER \
			|| data->mtyp == PM_PARASITIZED_DOLL \
		)

#define	hexWarded(data)	((is_minion(data) \
			|| data->mtyp == PM_HELL_HOUND \
			|| data->mtyp == PM_HELL_HOUND_PUP \
			|| data->mtyp == PM_EYE_OF_DOOM \
			|| data->mtyp == PM_SON_OF_TYPHON \
			|| is_golem(data) \
			|| is_angel(data) \
			|| is_keter(data) \
			|| data->mlet == S_QUANTMECH \
			|| data->mlet == S_IMP \
			|| is_demon(data) \
		) && !( \
			(data->mtyp == PM_CHOKHMAH_SEPHIRAH) || \
			(data->mtyp == PM_ELDER_PRIEST) || \
			(data->mtyp == PM_GREAT_CTHULHU) \
		))

#define	pentWarded(data)	((is_demon(data) \
			|| data->mtyp == PM_HELL_HOUND \
			|| data->mtyp == PM_HELL_HOUND_PUP \
			|| data->mtyp == PM_GARGOYLE \
			|| data->mtyp == PM_WINGED_GARGOYLE \
			|| data->mtyp == PM_DJINNI \
			|| data->mtyp == PM_SANDESTIN \
			|| data->mtyp == PM_SALAMANDER \
			|| data->mlet == S_ELEMENTAL \
			|| data->mlet == S_IMP \
		) && !( \
			(data->mtyp == PM_CHOKHMAH_SEPHIRAH) || \
			(data->mtyp == PM_ELDER_PRIEST) || \
			(data->mtyp == PM_GREAT_CTHULHU) \
		))

#define	circleWarded(data)	((data->mtyp == PM_CERBERUS || is_undead(data)) \
			&& !(mtmp->mtyp == PM_MAANZECORIAN))

#define gorgUnwardable(mtmp)	(standardUnwardable(mtmp) || is_lminion(mtmp) || mtmp->mtyp == PM_ANGEL)

#define	gorgWarded(data)	(!mindless(data) \
			&& !is_great_old_one(data) \
			&& (data->mtyp != PM_CHOKHMAH_SEPHIRAH) \
			&& (data->mtyp != PM_ELDER_PRIEST) \
			&& (data->mtyp != PM_LUGRIBOSSK) \
			&& (data->mtyp != PM_MAANZECORIAN) \
		)

#define heptUnwardable(mtmp)	(standardUnwardable(mtmp) || mtmp->isgd || is_lminion(mtmp) || mtmp->mtyp == PM_ANGEL || mtmp->mpeaceful)

#define	heptWarded(data)	(( !(is_human(data) || is_elf(data) \
			|| is_drow(data) || is_dwarf(data) \
			|| is_gnome(data) || is_orc(data)) \
			|| is_undead(data) || is_were(data) \
		) && !( \
			data->mtyp == PM_CHOKHMAH_SEPHIRAH \
		))

#define yellowUnwardable(mtmp)	(standardUnwardable(mtmp) || mtmp->mpeaceful || (mtmp->mtyp == PM_STRANGER) || (mtmp->mtyp == PM_SUZERAIN) || (mtmp->mtyp == PM_ELDER_PRIEST) || (has_template(mtmp, YELLOW_TEMPLATE)))

#define	yellowWarded(data)	(!heptWarded(data) && !is_undead(data) && !is_were(data) && !is_primordial(data) )

#endif /* MONDATA_H */
