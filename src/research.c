
#include "hack.h"


STATIC_OVL int
dorotvomit()
{
	struct monst* mtmp = 0;
	int nx = u.ux, ny = u.uy;
	if(!(getdir((char *)0) && (u.dx || u.dy)))
		return MOVE_CANCELLED;
	int i = 2;
	while(i-- > 0){
		nx += u.dx; 
		ny += u.dy; 
		if(!isok(nx,ny) || !ZAP_POS(levl[nx][ny].typ))
			return MOVE_CANCELLED;
		if((mtmp = m_at(nx, ny)))
			break;
	}
	if(!mtmp)
		return MOVE_CANCELLED;
	struct attack theattk = { AT_VOMT, AD_DISE, 1, 1 };
	xmeleehity(&youmonst, mtmp, &theattk, (struct obj **)0, -1, 0, FALSE);
	make_sick(0L, (char *) 0, TRUE, SICK_VOMITABLE);
	if(!umechanoid) morehungry(20*get_uhungersizemod());
	
	return MOVE_STANDARD;
}

STATIC_OVL int
dorotsummon()
{
	if (throweffect()) {
		int nx = u.dx, ny = u.dy;
		if(!isok(nx,ny) || !ZAP_POS(levl[nx][ny].typ) || (m_u_at(nx, ny) != 0)){
			pline("That's blocked!");
			return MOVE_CANCELLED;
		}

		struct monst *clone = makemon(&mons[PM_BUTTERFLY_SWARM], nx, ny, MM_ESUM|MM_EDOG|MM_ADJACENTOK|NO_MINVENT|MM_NOCOUNTBIRTH);
		struct obj *wep;
		if(clone){
			initedog(clone);
			clone = christen_monst(clone, plname);
			if(uswapwep && u.twoweap){
				wep = duplicate_obj(uswapwep, FALSE);
				if(wep){
					mpickobj(clone, wep);
				}
			}
			if(uwep){
				wep = duplicate_obj(uwep, FALSE);
				if(wep){
					mpickobj(clone, wep);
				}
			}
			clone->m_lev = u.ulevel;
			clone->mhp = clone->mhpmax = clone->m_lev*8;
			m_dowear(clone, TRUE);
			init_mon_wield_item(clone);
			mark_mon_as_summoned(clone, &youmonst, 3, 0);
			m_level_up_intrinsic(clone);
		}
	}
	losepw(45);
	morehungry(45*get_uhungersizemod());
	flags.botl = 1;
	return MOVE_STANDARD;
}

int
doupgradeability()
{
	winid tmpwin;
	anything any;
	any.a_void = 0;         /* zero out all bits */
	menu_item *selected;
	int n = 0;
	char ch = 'a';

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);

	any.a_int = 1;
	if(check_rot(ROT_VOMIT) && (umechanoid || u.uhs < WEAK)){
		n++;
		add_menu(tmpwin, NO_GLYPH, &any , ch, 0, ATR_NONE,
			 "Vomit rot", MENU_UNSELECTED);
	}
	ch++;
	any.a_int = 2;
	if(check_rot(ROT_WINGS) && check_rot(ROT_CLONE) && u.uen >= 45){
		n++;
		add_menu(tmpwin, NO_GLYPH, &any , ch, 0, ATR_NONE,
			 "Send forth a phantom duplicate", MENU_UNSELECTED);
	}
	if(!n){
		destroy_nhwindow(tmpwin);
		return MOVE_CANCELLED;
	}

	end_menu(tmpwin, "Pick an ability:");
	n = select_menu(tmpwin, PICK_ONE, &selected);
	destroy_nhwindow(tmpwin);
	if(n <= 0){
		return MOVE_CANCELLED;
	}
	n = selected[0].item.a_int;
	free(selected);

	if(n == 1){
		return dorotvomit();
	}
	else if(n == 2){
		return dorotsummon();
	}
	return MOVE_CANCELLED;
}

