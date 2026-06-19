#ifndef XHITY_H
#define XHITY_H

#define MELEEHURT_LONGSLASH_MASK	0x0000000FL
#define MELEEHURT_FORCE_BLEED		0x00000010L
#define MELEEHURT_FORCE_CHECK_JOUST	0x00000020L
#define MELEEHURT_SUPER_SNEAK		0x00000040L
#define MELEEHURT_DOUBLE_DAMAGE		0x00000080L
#define MELEEHURT_SHOVE				0x00000100L
#define MELEEHURT_SHOCKWAVE			0x00000200L

/* macros to unify player and monster */
#define x(mon)				((mon)==&youmonst ? u.ux : (mon)->mx)
#define y(mon)				((mon)==&youmonst ? u.uy : (mon)->my)
#define trapped(mon)		((mon)==&youmonst ? u.utrap : (mon)->mtrapped)
#define cantmove(mon)		((mon)==&youmonst ? (multi<0 || u.usleep) : helpless((mon)))
#define mlev(mon)			((mon)==&youmonst ? (Upolyd ? mons[u.umonnum].mlevel : u.ulevel) : (mon)->m_lev)
#define hp(mon)				((mon)==&youmonst ? (Upolyd ? &(u.mh) : &(u.uhp)) : &((mon)->mhp))
#define hpmax(mon)			((mon)==&youmonst ? (Upolyd ? &(u.mhmax) : &(u.uhpmax)) : &((mon)->mhpmax))
#define Fire_res(mon)		((mon)==&youmonst ? Fire_resistance : resists_fire((mon)))
#define InvFire_res(mon)	(((mon)==&youmonst ? InvFire_resistance : resists_fire((mon))) || ward_at(x((mon)),y((mon))) == SIGIL_OF_CTHUGHA)
#define UseInvFire_res(mon)	(InvFire_res(mon) || check_res_engine(mon, AD_FIRE))
#define Cold_res(mon)		((mon)==&youmonst ? Cold_resistance : resists_cold((mon)))
#define InvCold_res(mon)	(((mon)==&youmonst ? InvCold_resistance : resists_cold((mon))) || ward_at(x((mon)),y((mon))) == BRAND_OF_ITHAQUA)
#define UseInvCold_res(mon)	(InvCold_res(mon) || check_res_engine(mon, AD_COLD))
#define Shock_res(mon)		((mon)==&youmonst ? Shock_resistance : resists_elec((mon)))
#define InvShock_res(mon)	(((mon)==&youmonst ? InvShock_resistance : resists_elec((mon))) || ward_at(x((mon)),y((mon))) == TRACERY_OF_KARAKAL)
#define UseInvShock_res(mon)	(InvShock_res(mon) || check_res_engine(mon, AD_ELEC))
#define Acid_res(mon)		((mon)==&youmonst ? Acid_resistance : resists_acid((mon)))
#define InvAcid_res(mon)	((mon)==&youmonst ? InvAcid_resistance : resists_acid((mon)))
#define HellFire_res(mon)	((mon)==&youmonst ? HellFire_resistance  : resists_hellfire((mon)))
#define HellCold_res(mon)	((mon)==&youmonst ? HellCold_resistance  : resists_hellcold((mon)))
#define HellShock_res(mon)	((mon)==&youmonst ? HellShock_resistance : resists_hellelec((mon)))
#define HellAcid_res(mon)	((mon)==&youmonst ? HellAcid_resistance  : resists_hellacid((mon)))
#define HolyFire_res(mon)	((mon)==&youmonst ? HolyFire_resistance  : resists_holyfire((mon)))
#define HolyCold_res(mon)	((mon)==&youmonst ? HolyCold_resistance  : resists_holycold((mon)))
#define HolyShock_res(mon)	((mon)==&youmonst ? HolyShock_resistance : resists_holyelec((mon)))
#define HolyAcid_res(mon)	((mon)==&youmonst ? HolyAcid_resistance  : resists_holyacid((mon)))
#define UseInvAcid_res(mon)	(InvAcid_res(mon) || check_res_engine(mon, AD_ACID))
#define Sleep_res(mon)		((mon)==&youmonst ? Sleep_resistance : resists_sleep((mon)))
#define Disint_res(mon)		((mon)==&youmonst ? Disint_resistance : resists_disint((mon)))
#define Poison_res(mon)		((mon)==&youmonst ? Poison_resistance : resists_poison((mon)))
#define Drain_res(mon)		((mon)==&youmonst ? Drain_resistance : resists_drli((mon)))
#define Sick_res(mon)		((mon)==&youmonst ? Sick_resistance : resists_sickness((mon)))
#define Stone_res(mon)		((mon)==&youmonst ? Stone_resistance : resists_ston((mon)))
#define Magic_res(mon)		((mon)==&youmonst ? Antimagic : resists_magm((mon)))
#define Dark_res(mon)		((mon)==&youmonst ? Dark_immune : dark_immune((mon)))
#define Dark_vuln(mon)		((mon)==&youmonst ? Mortal_race : mortal_race((mon)))
#define Half_phys(mon)		((mon)==&youmonst ? Half_physical_damage : mon_resistance((mon), HALF_PHDAM))
#define Half_spel(mon)		((mon)==&youmonst ? Half_spell_damage : mon_resistance((mon), HALF_SPDAM))
#define Change_res(mon)		((mon)==&youmonst ? Unchanging : mon_resistance((mon), UNCHANGING))
#define Breathless_res(mon)	((mon)==&youmonst ? Breathless : breathless_mon(mon))
#define Water_res(mon)		((mon)==&youmonst ? Waterproof : mon_resistance((mon), WATERPROOF))
#define Gaze_res(mon)		((mon)==&youmonst ? Gaze_immune : mon_resistance((mon), GAZE_RES))
#define Focused_aura(mon)		((mon)==&youmonst ? FocusAura : mon_resistance((mon), FOCUS_AURA))
#define ProtectItems(mon)		((mon)==&youmonst ? ProtItems : mon_resistance((mon), PROT_ITEMS))
#define creature_at(x,y)	(isok(x,y) ? MON_AT(x, y) ? level.monsters[x][y] : (x==u.ux && y==u.uy) ? &youmonst : (struct monst *)0 : (struct monst *)0)

#define FATAL_DAMAGE_MODIFIER 9001

#define VIS_MAGR	0x01	/* aggressor is clearly visible */
#define VIS_MDEF	0x02	/* defender is clearly visible */
#define VIS_NONE	0x04	/* you are aware of at least one of magr and mdef */

#define ATTACKCHECK_NONE		0x00	/* do not attack */
#define ATTACKCHECK_ATTACK		0x01	/* attack normally */
#define ATTACKCHECK_BLDTHRST	0x02	/* attack against the player's will */

#define SUBOUT_SPELLS	 1	/* Spellcasting attack instead (Five Fiends of Chaos1 and Gae and silverknight) */
#define SUBOUT_BAEL1	 2	/* Bael's Sword Archon attack chain */
#define SUBOUT_BAEL2	 3	/* Bael's marilith-hands attack chain */
#define SUBOUT_SPIRITS	 4	/* Player's bound spirits */
#define SUBOUT_BARB1	 5	/* 1st bit of barbarian bonus attacks */
#define SUBOUT_BARB2	 6	/* 2nd bit of barbarian bonus attacks, must directly precede the 1st bit */
#define SUBOUT_MAINWEPB	 7	/* Bonus attack caused by the wielded *mainhand* weapon */
#define SUBOUT_XWEP		 8	/* Made an offhand attack */
#define SUBOUT_GOATSPWN	 9	/* Goat spawn: seduction */
#define SUBOUT_GRAPPLE	10	/* Grappler's Grasp crushing damage */
#define SUBOUT_SCORPION	11	/* Scorpion Carapace's sting */
#define SUBOUT_LOLTH1	12	/* Lolth's 8 arm attack chain */
#define SUBOUT_MARIARM1	13	/* Mechanical armor attack 1 */
#define SUBOUT_MARIARM2	14	/* Mechanical armor attack 2 */
#define SUBOUT_SHUBTONG	15	/* Mind-stealing tongue attack */
#define SUBOUT_V_CLAWS1	16	/* Extra vermiurge claws 1 */
#define SUBOUT_V_CLAWS2	17	/* Extra vermiurge claws 2 */
#define SUBOUT_BRAINSUCK	18	/* Brain suckers */
#define SUBOUT_ROT_SPORES	19	/* Pasive spore attack */
#define SUBOUT_ROT_VOMIT	20	/* Vomit rot */
#define SUBOUT_ROT_STING	21	/* Rot stinger */
#define SUBOUT_PUSH	22	/* Push attack */
#define SUBOUT_VOMIT    23	/* Vomit attack */
#define SUBOUT_T_SPORES	24	/* Tiefling spores counterattack */
#define SUBOUT_T_FLAMES	25	/* Tiefling flames counterattack */
#define SUBOUT_T_COLD	26	/* Tiefling cold counterattack */
#define SUBOUT_T_ACID	27	/* Tiefling acid counterattack */
#define SUBOUT_T_POISON	28	/* Tiefling poison counterattack */
#define SUBOUT_T_BITE   29	/* Tiefling snake fangs attack */
#define SUBOUT_T_VAMPIRE 30	/* Tiefling vampire fangs attack */
#define SUBOUT_T_SPIDER 31	/* Tiefling spider fangs attack */
#define SUBOUT_T_COLD_TOUCH 32	/* Tiefling cold touch attack */
#define SUBOUT_T_DRAIN_TOUCH 33	/* Tiefling drain touch attack */
#define SUBOUT_T_FIRE_TOUCH 34	/* Tiefling fire touch attack */
#define SUBOUT_T_SHOCK_TOUCH 35	/* Tiefling shock touch attack */
#define SUBOUT_T_RAMS_HORNS    36	/* Tiefling horns attack */
#define SUBOUT_T_DEMON_HORNS  37	/* Tiefling demon horn attack */
#define SUBOUT_T_NIGHTHORN 38	/* Tiefling nightmare horn attack */
#define SUBOUT_T_ANTLERS   39	/* Tiefling antlers attack */
#define SUBOUT_T_BULL_HORNS 40	/* Tiefling bull horns attack */
#define SUBOUT_T_PARALYSIS_PASSIVE 41	/* Tiefling paralysis passive attack */
#define SUBOUT_T_THORN_HAIR	42	/* Tiefling thorn hair attack */
#define SUBOUT_T_SCORPION_TAIL	43	/* Tiefling scorpion tail attack */
#define SUBOUT_T_SEDUCE_STEAL    44	/* Tiefling seduction/item-stealing attack */
#define SUBOUT_T_SEDUCE_DISARM  45	/* Tiefling seduction/disarming attack */
#define SUBOUT_A_MAR_1  46	/* Aasimar extra arm 1 */
#define SUBOUT_A_MAR_2  47	/* Aasimar extra arm 2 */
#define SUBOUT_A_MAR_3  48	/* Aasimar extra arm 3 */
#define SUBOUT_A_MAR_4  49	/* Aasimar extra arm 4 */
#define SUBOUT_A_ACID_TOUCH 50	/* Aasimar acidic touch attack */
#define SUBOUT_A_SUCK  51	/* Aasimar priminal sucking attack */
#define MAX_SUBOUT		51
#define SUBOUT_ARRAY_SIZE (MAX_SUBOUT/(sizeof(int)*8)+1)

#define ATTKFLAG_FORCE_BLEED		0x00000001L
#define ATTKFLAG_FORCE_CHECK_JOUST	0x00000002L
#define ATTKFLAG_SUPER_SNEAK		0x00000004L
#define ATTKFLAG_DOUBLE_DAMAGE		0x00000008L
#define ATTKFLAG_SHOVE				0x00000010L
#define ATTKFLAG_SHOCKWAVE			0x00000020L

#define XYATKFLAG_MIRROR_ATK		0x00000001L
#endif
