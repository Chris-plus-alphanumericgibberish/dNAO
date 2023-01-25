#ifndef XHITY_H
#define XHITY_H

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
#define creature_at(x,y)	(isok(x,y) ? MON_AT(x, y) ? level.monsters[x][y] : (x==u.ux && y==u.uy) ? &youmonst : (struct monst *)0 : (struct monst *)0)

#define FATAL_DAMAGE_MODIFIER 9001

#define VIS_MAGR	0x01	/* aggressor is clearly visible */
#define VIS_MDEF	0x02	/* defender is clearly visible */
#define VIS_NONE	0x04	/* you are aware of at least one of magr and mdef */

#define ATTACKCHECK_NONE		0x00	/* do not attack */
#define ATTACKCHECK_ATTACK		0x01	/* attack normally */
#define ATTACKCHECK_BLDTHRST	0x02	/* attack against the player's will */

/* TODO: put these in their specified header files */
/* mondata.h */
#define is_holy_mon(mon)	(is_angel((mon)->data) || has_template(mon, ILLUMINATED))
#define is_unholy_mon(mon)	(is_demon((mon)->data))
#define is_unblessed_mon(mon)	(is_auton((mon)->data) || is_rilmani((mon)->data) || is_kamerel((mon)->data))
#endif
