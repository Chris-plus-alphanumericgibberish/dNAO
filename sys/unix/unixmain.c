/*	SCCS Id: @(#)unixmain.c	3.4	1997/01/22	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* main.c - Unix NetHack */

#include "hack.h"
#include "dlb.h"

#include <sys/stat.h>
#include <signal.h>
#include <pwd.h>
#ifndef O_RDONLY
#include <fcntl.h>
#endif

#if !defined(_BULL_SOURCE) && !defined(__sgi) && !defined(_M_UNIX)
# if !defined(SUNOS4) && !(defined(ULTRIX) && defined(__GNUC__))
#  if defined(POSIX_TYPES) || defined(SVR4) || defined(HPUX)
extern struct passwd *FDECL(getpwuid,(uid_t));
#  else
extern struct passwd *FDECL(getpwuid,(int));
#  endif
# endif
#endif
extern struct passwd *FDECL(getpwnam,(const char *));
#ifdef CHDIR
static void FDECL(chdirx, (const char *,BOOLEAN_P));
#endif /* CHDIR */
static boolean NDECL(whoami);
static void FDECL(process_options, (int, char **));

#ifdef _M_UNIX
extern void NDECL(check_sco_console);
extern void NDECL(init_sco_cons);
#endif
#ifdef __linux__
extern void NDECL(check_linux_console);
extern void NDECL(init_linux_cons);
#endif

static void NDECL(wd_message);
#ifdef WIZARD
static boolean wiz_error_flag = FALSE;
#endif

#ifdef G_SHEOL
#ifdef G_HELL
#ifdef G_NOHELL
#define HASSHEOL
#endif
#endif
#endif

static int extract_data(int argc, char* argv[]);
#ifdef HASSHEOL
static boolean pb_prohibited_by_generation_flags(struct permonst *ptr
        , int inhell, int insheol);
#endif

int
main(argc,argv)
int argc;
char *argv[];
{
        return extract_data(argc, argv);

	register int fd;
#ifdef CHDIR
	register char *dir;
#endif
	boolean exact_username;
#ifdef SIMPLE_MAIL
	char* e_simple = NULL;
#endif
#if defined(__APPLE__)
	/* special hack to change working directory to a resource fork when
	   running from finder --sam */
#define MAC_PATH_VALUE ".app/Contents/MacOS/"
	char mac_cwd[1024], *mac_exe = argv[0], *mac_tmp;
	int arg0_len = strlen(mac_exe), mac_tmp_len, mac_lhs_len=0;
	getcwd(mac_cwd, 1024);
	if(mac_exe[0] == '/' && !strcmp(mac_cwd, "/")) {
	    if((mac_exe = strrchr(mac_exe, '/')))
		mac_exe++;
	    else
		mac_exe = argv[0];
	    mac_tmp_len = (strlen(mac_exe) * 2) + strlen(MAC_PATH_VALUE);
	    if(mac_tmp_len <= arg0_len) {
		mac_tmp = malloc(mac_tmp_len + 1);
		sprintf(mac_tmp, "%s%s%s", mac_exe, MAC_PATH_VALUE, mac_exe);
		if(!strcmp(argv[0] + (arg0_len - mac_tmp_len), mac_tmp)) {
		    mac_lhs_len = (arg0_len - mac_tmp_len) + strlen(mac_exe) + 5;
		    if(mac_lhs_len > mac_tmp_len - 1)
			mac_tmp = realloc(mac_tmp, mac_lhs_len);
		    strncpy(mac_tmp, argv[0], mac_lhs_len);
		    mac_tmp[mac_lhs_len] = '\0';
		    chdir(mac_tmp);
		}
		free(mac_tmp);
	    }
	}
#endif

#ifdef SIMPLE_MAIL
	/* figure this out early */
	e_simple = nh_getenv("SIMPLEMAIL");
	iflags.simplemail = (e_simple ? 1 : 0);
#endif

	hname = argv[0];
	hackpid = getpid();
	(void) umask(0777 & ~FCMASK);

	choose_windows(DEFAULT_WINDOW_SYS);

#ifdef CHDIR			/* otherwise no chdir() */
	/*
	 * See if we must change directory to the playground.
	 * (Perhaps hack runs suid and playground is inaccessible
	 *  for the player.)
	 * The environment variable HACKDIR is overridden by a
	 *  -d command line option (must be the first option given)
	 */
	dir = nh_getenv("NETHACKDIR");
	if (!dir) dir = nh_getenv("HACKDIR");
#endif
	if(argc > 1) {
#ifdef CHDIR
	    if (!strncmp(argv[1], "-d", 2) && argv[1][2] != 'e') {
		/* avoid matching "-dec" for DECgraphics; since the man page
		 * says -d directory, hope nobody's using -desomething_else
		 */
		argc--;
		argv++;
		dir = argv[0]+2;
		if(*dir == '=' || *dir == ':') dir++;
		if(!*dir && argc > 1) {
			argc--;
			argv++;
			dir = argv[0];
		}
		if(!*dir)
		    error("Flag -d must be followed by a directory name.");
	    }
	    if (argc > 1)
#endif /* CHDIR */

	    /*
	     * Now we know the directory containing 'record' and
	     * may do a prscore().  Exclude `-style' - it's a Qt option.
	     */
	    if (!strncmp(argv[1], "-s", 2) && strncmp(argv[1], "-style", 6)) {
#ifdef CHDIR
		chdirx(dir,0);
#endif
		prscore(argc, argv);
		exit(EXIT_SUCCESS);
	    }
	}

	/*
	 * Change directories before we initialize the window system so
	 * we can find the tile file.
	 */
#ifdef CHDIR
	chdirx(dir,1);
#endif

#ifdef _M_UNIX
	check_sco_console();
#endif
#ifdef __linux__
	check_linux_console();
#endif
	initoptions();
	init_nhwindows(&argc,argv);
	exact_username = whoami();
#ifdef _M_UNIX
	init_sco_cons();
#endif
#ifdef __linux__
	init_linux_cons();
#endif

	/*
	 * It seems you really want to play.
	 */
	u.uhp = 1;	/* prevent RIP on early quits */

	process_options(argc, argv);	/* command line options */

#ifdef DEF_PAGER
	if(!(catmore = nh_getenv("HACKPAGER")) && !(catmore = nh_getenv("PAGER")))
		catmore = DEF_PAGER;
#endif
#ifdef MAIL
	getmailstatus();
#endif
/* #ifdef WIZARD
 * 	if (wizard)
 * 		Strcpy(plname, "wizard");
 * 	else
 * #endif */
	if(!*plname /*|| !strncmp(plname, "player", 4)
		      || !strncmp(plname, "games", 4)*/) {
		askname();
	} else if (exact_username) {
		/* guard against user names with hyphens in them */
		int len = strlen(plname);
		/* append the current role, if any, so that last dash is ours */
		if (++len < sizeof plname)
			(void)strncat(strcat(plname, "-"),
				      pl_character, sizeof plname - len - 1);
	}
	plnamesuffix();		/* strip suffix from name; calls askname() */
				/* again if suffix was whole name */
				/* accepts any suffix */
#ifdef WIZARD
	if(!wizard) {
#endif
		/*
		 * check for multiple games under the same name
		 * (if !locknum) or check max nr of players (otherwise)
		 */
		(void) signal(SIGQUIT,SIG_IGN);
		(void) signal(SIGINT,SIG_IGN);
		(void) signal(SIGHUP,SIG_IGN);
		if(!locknum)
			Sprintf(lock, "%d%s", (int)getuid(), plname);
		getlock();
#ifdef WIZARD
	} else {
		Sprintf(lock, "%d%s", (int)getuid(), plname);
		getlock();
	}
#endif /* WIZARD */

	(void) signal(SIGHUP, (SIG_RET_TYPE) hangup);
#ifdef SIGXCPU
	(void) signal(SIGXCPU, (SIG_RET_TYPE) hangup);
#endif

	dlb_init();	/* must be before newgame() */

	/*
	 * Initialization of the boundaries of the mazes
	 * Both boundaries have to be even.
	 */
	x_maze_max = COLNO-1;
	if (x_maze_max % 2)
		x_maze_max--;
	y_maze_max = ROWNO-1;
	if (y_maze_max % 2)
		y_maze_max--;

	/*
	 *  Initialize the vision system.  This must be before mklev() on a
	 *  new game or before a level restore on a saved game.
	 */
	vision_init();

	display_gamewindows();

	if ((fd = restore_saved_game()) >= 0) {
#ifdef WIZARD
		/* Since wizard is actually flags.debug, restoring might
		 * overwrite it.
		 */
		boolean remember_wiz_mode = wizard;
#endif
		const char *fq_save = fqname(SAVEF, SAVEPREFIX, 1);

		(void) chmod(fq_save,0);	/* disallow parallel restores */
		(void) signal(SIGINT, (SIG_RET_TYPE) done1);
#ifdef NEWS
		if(iflags.news) {
		    display_file(NEWS, FALSE);
		    iflags.news = FALSE; /* in case dorecover() fails */
		}
#endif
		pline("Restoring save file...");
		mark_synch();	/* flush output */
		if(!dorecover(fd))
			goto not_recovered;
#ifdef WIZARD
		if(!wizard && remember_wiz_mode) wizard = TRUE;
#endif
		check_special_room(FALSE);
		wd_message();

		if (discover || wizard) {
			if(yn("Do you want to keep the save file?") == 'n')
			    (void) delete_savefile();
			else {
			    (void) chmod(fq_save,FCMASK); /* back to readable */
			    compress(fq_save);
			}
		}
		flags.move = 0;
	} else {
not_recovered:
		player_selection();
		newgame();
		wd_message();

		flags.move = 0;
		set_wear();
		(void) pickup(1);
	}

	moveloop();
	exit(EXIT_SUCCESS);
	/*NOTREACHED*/
	return(0);
}

static void
process_options(argc, argv)
int argc;
char *argv[];
{
	int i;


	/*
	 * Process options.
	 */
	while(argc > 1 && argv[1][0] == '-'){
		argv++;
		argc--;
		switch(argv[0][1]){
		case 'D':
#ifdef WIZARD
			wizard = TRUE;
			break;
#endif
		case 'X':
			discover = TRUE;
			break;
#ifdef NEWS
		case 'n':
			iflags.news = FALSE;
			break;
#endif
		case 'u':
			if(argv[0][2])
			  (void) strncpy(plname, argv[0]+2, sizeof(plname)-1);
			else if(argc > 1) {
			  argc--;
			  argv++;
			  (void) strncpy(plname, argv[0], sizeof(plname)-1);
			} else
				raw_print("Player name expected after -u");
			break;
		case 'I':
		case 'i':
			if (!strncmpi(argv[0]+1, "IBM", 3))
				switch_graphics(IBM_GRAPHICS);
			break;
	    /*  case 'D': */
		case 'd':
			if (!strncmpi(argv[0]+1, "DEC", 3))
				switch_graphics(DEC_GRAPHICS);
			break;
		case 'p': /* profession (role) */
			if (argv[0][2]) {
			    if ((i = str2role(&argv[0][2])) >= 0)
			    	flags.initrole = i;
			} else if (argc > 1) {
				argc--;
				argv++;
			    if ((i = str2role(argv[0])) >= 0)
			    	flags.initrole = i;
			}
			break;
		case 'r': /* race */
			if (argv[0][2]) {
			    if ((i = str2race(&argv[0][2])) >= 0)
			    	flags.initrace = i;
			} else if (argc > 1) {
				argc--;
				argv++;
			    if ((i = str2race(argv[0])) >= 0)
			    	flags.initrace = i;
			}
			break;
		case '@':
			flags.randomall = 1;
			break;
		default:
			if ((i = str2role(&argv[0][1])) >= 0) {
			    flags.initrole = i;
				break;
			}
			/* else raw_printf("Unknown option: %s", *argv); */
		}
	}

	if(argc > 1)
		locknum = atoi(argv[1]);
#ifdef MAX_NR_OF_PLAYERS
	if(!locknum || locknum > MAX_NR_OF_PLAYERS)
		locknum = MAX_NR_OF_PLAYERS;
#endif
}

#ifdef CHDIR
static void
chdirx(dir, wr)
const char *dir;
boolean wr;
{
	if (dir					/* User specified directory? */
# ifdef HACKDIR
	       && strcmp(dir, HACKDIR)		/* and not the default? */
# endif
		) {
# ifdef SECURE
	    (void) setgid(getgid());
	    (void) setuid(getuid());		/* Ron Wessels */
# endif
	} else {
	    /* non-default data files is a sign that scores may not be
	     * compatible, or perhaps that a binary not fitting this
	     * system's layout is being used.
	     */
# ifdef VAR_PLAYGROUND
	    int len = strlen(VAR_PLAYGROUND);

	    fqn_prefix[SCOREPREFIX] = (char *)alloc(len+2);
	    Strcpy(fqn_prefix[SCOREPREFIX], VAR_PLAYGROUND);
	    if (fqn_prefix[SCOREPREFIX][len-1] != '/') {
		fqn_prefix[SCOREPREFIX][len] = '/';
		fqn_prefix[SCOREPREFIX][len+1] = '\0';
	    }
# endif
	}

# ifdef HACKDIR
	if (dir == (const char *)0)
	    dir = HACKDIR;
# endif

	if (dir && chdir(dir) < 0) {
	    perror(dir);
	    error("Cannot chdir to %s.", dir);
	}

	/* warn the player if we can't write the record file */
	/* perhaps we should also test whether . is writable */
	/* unfortunately the access system-call is worthless */
	if (wr) {
# ifdef VAR_PLAYGROUND
	    fqn_prefix[LEVELPREFIX] = fqn_prefix[SCOREPREFIX];
	    fqn_prefix[SAVEPREFIX] = fqn_prefix[SCOREPREFIX];
	    fqn_prefix[BONESPREFIX] = fqn_prefix[SCOREPREFIX];
	    fqn_prefix[LOCKPREFIX] = fqn_prefix[SCOREPREFIX];
	    fqn_prefix[TROUBLEPREFIX] = fqn_prefix[SCOREPREFIX];
# endif
	    check_recordfile(dir);
	}
}
#endif /* CHDIR */

static boolean
whoami() {
	/*
	 * Who am i? Algorithm: 1. Use name as specified in NETHACKOPTIONS
	 *			2. Use $USER or $LOGNAME	(if 1. fails)
	 *			3. Use getlogin()		(if 2. fails)
	 * The resulting name is overridden by command line options.
	 * If everything fails, or if the resulting name is some generic
	 * account like "games", "play", "player", "hack" then eventually
	 * we'll ask him.
	 * Note that we trust the user here; it is possible to play under
	 * somebody else's name.
	 */
	register char *s;

	if (*plname) return FALSE;
	if(/* !*plname && */ (s = nh_getenv("USER")))
		(void) strncpy(plname, s, sizeof(plname)-1);
	if(!*plname && (s = nh_getenv("LOGNAME")))
		(void) strncpy(plname, s, sizeof(plname)-1);
	if(!*plname && (s = getlogin()))
		(void) strncpy(plname, s, sizeof(plname)-1);
	return TRUE;
}

#ifdef PORT_HELP
void
port_help()
{
	/*
	 * Display unix-specific help.   Just show contents of the helpfile
	 * named by PORT_HELP.
	 */
	display_file(PORT_HELP, TRUE);
}
#endif

static void
wd_message()
{
#ifdef WIZARD
	if (wiz_error_flag) {
		pline("Only user \"%s\" may access debug (wizard) mode.",
# ifndef KR1ED
			WIZARD);
# else
			WIZARD_NAME);
# endif
		pline("Entering discovery mode instead.");
	} else
#endif
	if (discover)
		You("are in non-scoring discovery mode.");
}

/*
 * Add a slash to any name not ending in /. There must
 * be room for the /
 */
void
append_slash(name)
char *name;
{
	char *ptr;

	if (!*name)
		return;
	ptr = name + (strlen(name) - 1);
	if (*ptr != '/') {
		*++ptr = '/';
		*++ptr = '\0';
	}
	return;
}





/*** EXTRACTOR STUFF ***/
#ifndef DNETHACK
#include "date.h"
#endif
static void extract_monsterdata_to_yaml(
        FILE* f
      , const char* variant
      , const char* command_prefix );
static const char* detect_variant( const char** command_prefix );

static int extract_data(int argc, char* argv[])
{
    FILE* f;
    char name[300];
    const char* command_prefix, *variant_str;

    if (argc <= 1)
    {
        printf("Usage:\n");
        printf("%s [language]\n\n", argv[0]);
        printf("Where language is one of: \n");
        printf("  yaml\n\n");
        return 0;
    }

    variant_str = detect_variant( &command_prefix );
    sprintf( name, "%s.yaml", variant_str );
    if (argc >= 2 && !strcmp(argv[1], "yaml"))
    {
        fprintf( stderr, "Writing to '%s'...\n", variant_str );
        f = fopen(name, "wb");
        if (!f)
        {
            perror("fopen");
            return -1;
        }
        extract_monsterdata_to_yaml(f, variant_str, command_prefix );
        fclose(f);
        fprintf( stderr, "Done.\n" );
    }
    else
    {
        fprintf(stderr,
               "I don't what %s is. Try running %s without "
               "parameters to see the valid languages.\n",
               argv[1], argv[0]);
        return -1;
    }

    return 0;
}

static const char* detect_variant( const char** command_prefix )
{
    const char* vid = VERSION_ID;
    if ( strstri( vid, "UnNetHackPlus" ) ) {
        (*command_prefix) = "u+";
        return "UnNetHackPlus";
    }
    if ( strstri( vid, "UnNetHack" ) ) {
        (*command_prefix) = "u";
        return "UnNetHack";
    }
    if ( strstri( vid, "SporkHack" ) ) {
        (*command_prefix) = "s";
        return "SporkHack";
    }
    if ( strstri( vid, "NetHack Version 3.4.3" ) ) {
        (*command_prefix) = "v";
        return "Vanilla";
    }

    fprintf( stderr
           , "I don't know what variant this is. "
           "Replace module, data type name and command prefix manually. "
           "Search for 'ReplaceThisName' in the resulting source file.\n" );

    (*command_prefix) = "ReplaceThisName";

    return "ReplaceThisName";
}

extern const int monstr[];

static void extract_monsterdata_to_yaml(
        FILE* f
      , const char* variant
      , const char* command_prefix )
{
    int i1, i2;
    struct permonst* pm;
    struct monst dummymonst;

    memset(&dummymonst, 0, sizeof(dummymonst));

    /* We want to import everything in this module. */
    fprintf(f, "variant: \"%s\"\n", variant );
    fprintf(f, "prefix: \"%s\"\n\n", command_prefix );

    fprintf(f, "monsters:\n");

    for (i1 = 0; mons[i1].mname[0]; ++i1)
    {
        pm = &mons[i1];
        dummymonst.data = pm;

        if (i1 > 0) fprintf(f, "\n");

        fprintf(f, " - name: \"%s\"\n", pm->mname);
        fprintf(f, "   symbol: \"%c\"\n", def_monsyms[pm->mlet]);
        fprintf(f, "   base-level: %d\n", pm->mlevel);
        fprintf(f, "   difficulty: %d\n", monstr[(monsndx(pm))]);
        fprintf(f, "   speed: %d\n", pm->mmove);
        fprintf(f, "   ac: %d\n", pm->ac);
        fprintf(f, "   mr: %d\n", pm->mr);
        fprintf(f, "   alignment: %d\n", pm->maligntyp);
        fprintf(f, "   generates:\n");
        if (pm->geno & G_UNIQ)
            fprintf(f, "    - unique\n");
        else if (pm->geno & G_NOGEN)
            fprintf(f, "");
        else
        {
#ifdef HASSHEOL
            if ( !pb_prohibited_by_generation_flags( pm, 1, 1 ) )
                fprintf(f, "    - sheol\n");
            if ( !pb_prohibited_by_generation_flags( pm, 1, 0 ) )
                fprintf(f, "    - gehennom\n");
            if ( !pb_prohibited_by_generation_flags( pm, 0, 0 ) )
                fprintf(f, "    - dungeons\n");
#else
#ifdef G_HELL
#ifdef G_NOHELL
            if ( (pm->geno & G_HELL ||
                  !(pm->geno & G_NOHELL)) ) fprintf(f, "    - gehennom\n");
            if ( !(pm->geno & G_HELL) )
                 fprintf(f, "    - dungeons\n");
#endif
#endif
#endif
        }
        fprintf(f, "   leaves-corpse: %s\n",
                (pm->geno & G_NOCORPSE) ? "No" : "Yes");
        fprintf(f, "   not-generated-normally: %s\n",
                (pm->geno & G_NOGEN) ? "Yes" : "No");
        fprintf(f, "   appears-in-small-groups: %s\n",
                (pm->geno & G_SGROUP) ? "Yes" : "No");
        fprintf(f, "   appears-in-large-groups: %s\n",
                (pm->geno & G_LGROUP) ? "Yes" : "No");
        fprintf(f, "   genocidable: %s\n",
                (pm->geno & G_GENO) ? "Yes" : "No");
        fprintf(f, "   attacks: [");
        for (i2 = 0; i2 < NATTK && (pm->mattk[i2].aatyp ||
                                    pm->mattk[i2].adtyp ||
                                    pm->mattk[i2].damn ||
                                    pm->mattk[i2].damd); ++i2)
        {
            if (i2 > 0)
                fprintf(f, ", ");

            fprintf(f, "[");
#define AT(a, b) else if (pm->mattk[i2].aatyp == a) fprintf(f, "%s", b);
            if (1 + 1 == 3) { } /* I hope we won't run this code 
                                   in a universe where 1+1 is 3. */
            AT(AT_NONE, "AtNone")
            AT(AT_CLAW, "AtClaw")
            AT(AT_BITE, "AtBite")
            AT(AT_KICK, "AtKick")
            AT(AT_BUTT, "AtButt")
            AT(AT_TUCH, "AtTouch")
            AT(AT_STNG, "AtSting")
            AT(AT_HUGS, "AtHug")
            AT(AT_SPIT, "AtSpit")
            AT(AT_ENGL, "AtEngulf")
            AT(AT_BREA, "AtBreath")
            AT(AT_EXPL, "AtExplode")
            AT(AT_BOOM, "AtSuicideExplode")
            AT(AT_GAZE, "AtGaze")
            AT(AT_TENT, "AtTentacle")
            AT(AT_WEAP, "AtWeapon")
            AT(AT_MAGC, "AtCast")
#ifdef AT_SCRA
            AT(AT_SCRA, "AtScratch")
#endif
#ifdef AT_LASH
            AT(AT_LASH, "AtLash")
#endif
#ifdef AT_TRAM
            AT(AT_TRAM, "AtTrample")
#endif
/* dnethack */
#ifdef AT_ARRW
            AT(AT_ARRW, "AtArrow")
            AT(AT_WHIP, "AtWhip")
            AT(AT_LRCH, "AtReach")
            AT(AT_HODS, "AtMirror")
            AT(AT_LNCK, "AtReachingBite")
            AT(AT_MMGC, "AtMMagical")
            AT(AT_ILUR, "AtIllurien")
            AT(AT_HITS, "AtAutoHit")
            AT(AT_WISP, "AtWispMist")
            AT(AT_TNKR, "AtTinker")
#endif
#ifdef AT_SHDW
            AT(AT_SHDW, "AtPhaseNonContact")
#endif
#ifdef AT_BEAM
            AT(AT_BEAM, "AtBeamNonContact")
#endif
#ifdef AT_DEVA
            AT(AT_DEVA, "AtMillionArms")
#endif
#ifdef AT_MULTIPLY
            AT(AT_MULTIPLY, "AtMultiply")
#endif
#ifdef AT_SCRE
            AT(AT_SCRE, "AtScre")
#endif
            else { fprintf(stderr,
                    "I don't know what attack type %d is.\n", pm->mattk[i2].aatyp);
                   abort(); }
#undef AT
#define AT(a, b) else if (pm->mattk[i2].adtyp == a) fprintf(f, ", %s", b);
            if (1 == 2) { }
            AT(AD_PHYS, "AdPhys")
            AT(AD_MAGM, "AdMagicMissile")
            AT(AD_FIRE, "AdFire")
            AT(AD_COLD, "AdCold")
            AT(AD_SLEE, "AdSleep")
            AT(AD_DISN, "AdDisintegrate")
            AT(AD_ELEC, "AdElectricity")
            AT(AD_DRST, "AdStrDrain")
            AT(AD_ACID, "AdAcid")
            AT(AD_BLND, "AdBlind")
            AT(AD_STUN, "AdStun")
            AT(AD_SLOW, "AdSlow")
            AT(AD_PLYS, "AdParalyse")
            AT(AD_DRLI, "AdLevelDrain")
            AT(AD_DREN, "AdMagicDrain")
            AT(AD_LEGS, "AdLegs")
            AT(AD_STON, "AdStone")
            AT(AD_STCK, "AdSticking")
            AT(AD_SGLD, "AdGoldSteal")
            AT(AD_SITM, "AdItemSteal")
            AT(AD_SEDU, "AdSeduce")
            AT(AD_TLPT, "AdTeleport")
            AT(AD_RUST, "AdRust")
            AT(AD_CONF, "AdConfuse")
            AT(AD_DGST, "AdDigest")
            AT(AD_HEAL, "AdHeal")
            AT(AD_WRAP, "AdWrap")
            AT(AD_WERE, "AdWere")
            AT(AD_DRDX, "AdDexDrain")
            AT(AD_DRCO, "AdConDrain")
            AT(AD_DRIN, "AdIntDrain")
            AT(AD_DISE, "AdDisease")
            AT(AD_DCAY, "AdRot")
            AT(AD_SSEX, "AdSex")
            AT(AD_HALU, "AdHallucination")
            AT(AD_DETH, "AdDeath")
            AT(AD_PEST, "AdPestilence")
            AT(AD_FAMN, "AdFamine")
            AT(AD_SLIM, "AdSlime")
            AT(AD_ENCH, "AdDisenchant")
            AT(AD_CORR, "AdCorrode")
            AT(AD_CLRC, "AdClerical")
            AT(AD_SPEL, "AdSpell")
            AT(AD_RBRE, "AdRandomBreath")
            AT(AD_SAMU, "AdAmuletSteal")
            AT(AD_CURS, "AdCurse")
#ifdef AD_VAMP
            AT(AD_VAMP, "AdVampireDrain")
#endif
#ifdef AD_DEAD
            AT(AD_DEAD, "AdDeadGaze")
#endif
#ifdef AD_SUCK
            AT(AD_SUCK, "AdSuckEquipment")
#endif
#ifdef AD_CHRN
            AT(AD_CHRN, "AdCursedUnihorn")
#endif
#ifdef AD_RGAZ
            AT(AD_RGAZ, "AdRandomGaze")
#endif
#ifdef AD_LITE
            AT(AD_LITE, "AdLightRay")
#endif
#ifdef AD_NGRA
            AT(AD_NGRA, "AdRemoveEngravings")
#endif
#ifdef AD_GLIB
            AT(AD_GLIB, "AdDisarm")
#endif
#ifdef AD_DARK
            AT(AD_DARK, "AdRemoveLight")
#endif
#ifdef AD_ENDS
            AT(AD_ENDS, "AdPlaceholder")
#endif
#ifdef AD_WTHR
            AT(AD_WTHR, "AdWither")
#endif
#ifdef AD_SHRD
            AT(AD_SHRD, "AdShred")
#endif
#ifdef AD_CHKH
            AT(AD_CHKH, "AdEscalatingDamage")
#endif
#ifdef AD_NEXU
            AT(AD_NEXU, "AdNexus")
#endif
#ifdef AD_INER
            AT(AD_INER, "AdInertia")
#endif
#ifdef AD_TIME
            AT(AD_TIME, "AdDrainLifeOrStats")
#endif
#ifdef AD_PLAS
            AT(AD_PLAS, "AdPlasma")
#endif
#ifdef AD_GRAV
            AT(AD_GRAV, "AdGravity")
#endif
#ifdef AD_ABDC
            AT(AD_ABDC, "AdAbduction")
#endif
#ifdef AD_UVUU
            AT(AD_UVUU, "AdHeadSpike")
#endif
#ifdef AD_HODS
            AT(AD_HODS, "AdMirror")
#endif
#ifdef AD_AXUS
            AT(AD_AXUS, "AdMultiElementCounterAttackThatAngersTons")
#endif
#ifdef AD_DFOO
            AT(AD_DFOO, "AdDrainsAllSortsOfStuff")
#endif
#ifdef AD_WET
            AT(AD_WET, "AdWet")
#endif
#ifdef AD_FAKE
            AT(AD_FAKE, "AdFakeMessages")
#endif
#ifdef AD_THIR
            AT(AD_THIR, "AdThirsty")
#endif
#ifdef AD_WEEP
            AT(AD_WEEP, "AdWeeping")
#endif
#ifdef AD_WEBS
            AT(AD_WEBS, "AdWebs")
#endif
#ifdef AD_SOUN
            AT(AD_SOUN, "AdSound")
#endif
#ifdef AD_LETHE
            AT(AD_LETHE, "AdLethe")
#endif
#ifdef AD_LETH
            AT(AD_LETH, "AdLethe")
#endif
#ifdef AD_WISD
            AT(AD_WISD, "AdWisdom")
#endif
#ifdef AD_MALK
            AT(AD_MALK, "AdMalk")
#endif
#ifdef AD_BANI
            AT(AD_BANI, "AdBanishment")
#endif
#ifdef AD_WRAT
            AT(AD_WRAT, "AdWrath")
#endif
#ifdef AD_NPRO
            AT(AD_NPRO, "AdNegativeProtection")
#endif
#ifdef AD_DEPR
            AT(AD_DEPR, "AdDepression")
#endif
#ifdef AD_LAZY
            AT(AD_LAZY, "AdLazyness")
#endif
#ifdef AD_MANA
            AT(AD_MANA, "AdMana")
#endif
#ifdef AD_DRCH
            AT(AD_DRCH, "AdCharisma")
#endif
/* dnethack, bundled together */
#ifdef AD_UNKNWN
            AT(AD_VORP, "AdVorpal")
            AT(AD_BIST, "AdBisectBeak")
            AT(AD_MIST, "AdMist")
            AT(AD_SUCK, "AdSuck")
            AT(AD_GROW, "AdGrow")
            AT(AD_SOUL, "AdSoul")
            AT(AD_TELE, "AdTele")
            AT(AD_CHRN, "AdHorn")
            AT(AD_SOLR, "AdSolar")
            AT(AD_SLVR, "AdSilver")
            AT(AD_BALL, "AdIronBall")
            AT(AD_RETR, "AdElementalGaze")
            AT(AD_TENT, "AdTentacle")
            AT(AD_UNKNWN, "AdUnknownPriest")
            AT(AD_POSN, "AdPoison")
            AT(AD_SPNL, "AdLeviathan")
            AT(AD_HLBD, "AdAsmodeusBlood")
            AT(AD_SQUE, "AdStealQuestArtifact")
            AT(AD_KAOS, "AdSpawnChaos")
            AT(AD_WISD, "AdWisdom")
            AT(AD_BLDR, "AdBoulderArrow")
            AT(AD_VBLD, "AdBoulderArrowRandomSpread")
            AT(AD_JAILER, "AdJailer")
            AT(AD_VAMP, "AdVamp")
            AT(AD_BARB, "AdPhysRetaliate")
            AT(AD_GARO, "AdGaro")
            AT(AD_GARO_MASTER, "AdGaroMaster")
            AT(AD_SSUN, "AdSunflower")
            AT(AD_FNEX, "AdFernExplosion")
            AT(AD_MAND, "AdMandrake")
            AT(AD_LOAD, "AdLoadstones")
            AT(AD_ILUR, "AdIllurien")
            AT(AD_TNKR, "AdTinker")
            AT(AD_FRWK, "AdFireworks")
            AT(AD_STDY, "AdStudy")
            AT(AD_DUNSTAN, "AdDunstan")
            AT(AD_IRIS, "AdIris")
            AT(AD_NABERIUS, "AdNaberius")
            AT(AD_OTIAX, "AdOtiax")
            AT(AD_SIMURGH, "AdSimurgh")
            AT(AD_CMSL, "AdColdMissile")
            AT(AD_FMSL, "AdFireMissile")
            AT(AD_EMSL, "AdElectricMissile")
            AT(AD_SMSL, "AdPhysicalShrapnel")
            AT(AD_WMTG, "AdWarMachineGaze")
#endif
#ifdef AD_SPOR
            AT(AD_SPOR, "AdSpore")
#endif
#ifdef AD_LAVA
            AT(AD_LAVA, "AdLava")
#endif
#ifdef AD_CALM
            AT(AD_CALM, "AdCalm")
#endif
#ifdef AD_TCKL
            AT(AD_TCKL, "AdTickle")
#endif
#ifdef AD_POLY
            AT(AD_POLY, "AdPoly")
#endif
#ifdef AD_BHED
            AT(AD_BHED, "AdBehead")
#endif
#ifdef AD_CNCL
            AT(AD_CNCL, "AdCancellation")
#endif
#ifdef AD_LVLT
            AT(AD_LVLT, "AdLevelTeleport")
            AT(AD_BLNK, "AdBlink")
#endif
#ifdef AD_DRWN
            AT(AD_DRWN, "AdDrown")
#endif
#ifdef G_SHEOL
            AT(AD_FREZ, "AdFreeze")
            AT(AD_PUNI, "AdPunisher")
#endif
#ifdef AD_HEAD
            AT(AD_HEAD, "AdDecapitate")
#endif
#ifdef AD_LUCK
            AT(AD_LUCK, "AdDrainLuck")
#endif
#ifdef AD_OONA
            AT(AD_OONA, "AdOona")
#endif
#ifdef AD_NTZC
            AT(AD_NTZC, "AdNetzach")
#endif
#ifdef AD_WTCH
            AT(AD_WTCH, "AdWatcherTentacleGaze")
#endif
#ifdef AD_STTP
            AT(AD_STTP, "AdStealByTeleportation")
#endif
#ifdef AD_HDRG
            AT(AD_HDRG, "AdHalfDragon")
#endif
#ifdef AD_STAR
            AT(AD_STAR, "AdSilverStarlightRapier")
#endif
#ifdef AD_SHDW
            AT(AD_SHDW, "AdBlackWebShadow")
#endif
#ifdef AD_SPORE
            AT(AD_SPORE, "AdSpore")
#endif
#ifdef AD_NUMB
            AT(AD_NUMB, "AdNumb")
#endif
#ifdef AD_FRZE
            AT(AD_FRZE, "AdFreezeSolid")
#endif
#ifdef AD_POIS
            AT(AD_POIS, "AdPoisonStat")
#endif
#ifdef AD_DISP
            AT(AD_DISP, "AdDisplacement")
#endif
#ifdef AD_BURN
            AT(AD_BURN, "AdBurn")
#endif
#ifdef AD_FEAR
            AT(AD_FEAR, "AdFear")
#endif
#ifdef AD_NAST
            AT(AD_NAST, "AdNastyTrap")
#endif
#ifdef AD_SKIL
            AT(AD_SKIL, "AdSkillCapReduce")
#endif
#ifdef AD_DREA
            AT(AD_DREA, "AdDreamAttack")
#endif
#ifdef AD_BADE
            AT(AD_BADE, "AdBadRandomEffect")
#endif
#ifdef AD_FUMB
            AT(AD_FUMB, "AdFumble")
#endif
#ifdef AD_VENO
            AT(AD_VENO, "AdVenomous")
#endif
#ifdef AD_VULN
            AT(AD_VULN, "AdVulnerability")
#endif
#ifdef AD_ICUR
            AT(AD_ICUR, "AdCurseItems")
#endif
#ifdef AD_SLUD
            AT(AD_SLUD, "AdSludge")
#endif
#ifdef AD_SPC2
            AT(AD_SPC2, "AdMasterBlaster")
#endif
            else { fprintf(stderr,
                    "I don't know what attack damage type %d is. (%s)\n",
                    pm->mattk[i2].adtyp, pm->mname); abort(); }
#undef AT
            fprintf(f, ", %d, %d]", pm->mattk[i2].damn, pm->mattk[i2].damd);
        }
        fprintf(f, "]\n");
        fprintf(f, "   weight: %d\n", pm->cwt);
        fprintf(f, "   nutrition: %d\n", pm->cnutrit);
        fprintf(f, "   size: ");
        if (pm->msize == MZ_TINY) fprintf(f, "tiny\n");
        else if (pm->msize == MZ_SMALL) fprintf(f, "small\n");
        else if (pm->msize == MZ_MEDIUM) fprintf(f, "medium\n");
        else if (pm->msize == MZ_LARGE) fprintf(f, "large\n");
        else if (pm->msize == MZ_HUGE) fprintf(f, "huge\n");
        else if (pm->msize == MZ_GIGANTIC) fprintf(f, "gigantic\n");
        else { fprintf(stderr,
                "I don't know what size %d means.\n", pm->msize); abort(); }

        fprintf(f, "   resistances:\n");
        if (pm->mresists & MR_FIRE) fprintf(f, "    - ReFire\n");
        if (pm->mresists & MR_COLD) fprintf(f, "    - ReCold\n");
        if (pm->mresists & MR_SLEEP) fprintf(f, "    - ReSleep\n");
        if (pm->mresists & MR_DISINT) fprintf(f, "    - ReDisintegrate\n");
        if (pm->mresists & MR_ELEC) fprintf(f, "    - ReElectricity\n");
        if (pm->mresists & MR_POISON) fprintf(f, "    - RePoison\n");
        if (pm->mresists & MR_ACID) fprintf(f, "    - ReAcid\n");
        if (pm->mresists & MR_STONE) fprintf(f, "    - RePetrification\n");
        if (resists_magm(&dummymonst)) fprintf(f, "    - ReMagic\n");
        if (resists_drli(&dummymonst)) fprintf(f, "    - ReDrain\n");
        fprintf(f, "   conferred:\n");
        if (pm->mconveys & MR_FIRE) fprintf(f, "    - ReFire\n");
        if (pm->mconveys & MR_COLD) fprintf(f, "    - ReCold\n");
        if (pm->mconveys & MR_SLEEP) fprintf(f, "    - ReSleep\n");
        if (pm->mconveys & MR_DISINT) fprintf(f, "    - ReDisintegrate\n");
        if (pm->mconveys & MR_ELEC) fprintf(f, "    - ReElectricity\n");
        if (pm->mconveys & MR_POISON) fprintf(f, "    - RePoison\n");
        if (pm->mconveys & MR_ACID) fprintf(f, "    - ReAcid\n");
        /* You can't actually get petrification resistance. */
        // if (pm->mconveys & MR_STONE) fprintf(f, "    - RePetrification\n");

        fprintf(f, "   flags: [");
        {
            int comma_set = 0;
#ifndef DNETHACK
#define AT(a, b) if (pm->mflags1 & a) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", b); \
}
        AT(M1_FLY, "FlFly");
        AT(M1_SWIM, "FlSwim");
        AT(M1_AMORPHOUS, "FlAmorphous");
        AT(M1_WALLWALK, "FlWallwalk");
        AT(M1_CLING, "FlCling");
        AT(M1_TUNNEL, "FlTunnel");
        AT(M1_NEEDPICK, "FlNeedPick");
        AT(M1_CONCEAL, "FlConceal");
        AT(M1_HIDE, "FlHide");
        AT(M1_AMPHIBIOUS, "FlAmphibious");
        AT(M1_BREATHLESS, "FlBreathless");
        AT(M1_NOTAKE, "FlNoTake");
        AT(M1_NOEYES, "FlNoEyes");
        AT(M1_NOHANDS, "FlNoHands");
        AT(M1_NOLIMBS, "FlNoLimbs");
        AT(M1_NOHEAD, "FlNoHead");
        AT(M1_MINDLESS, "FlMindless");
        AT(M1_HUMANOID, "FlHumanoid");
        AT(M1_ANIMAL, "FlAnimal");
        AT(M1_SLITHY, "FlSlithy");
        AT(M1_UNSOLID, "FlUnSolid");
        AT(M1_THICK_HIDE, "FlThickHide");
        AT(M1_OVIPAROUS, "FlOviparous");
        AT(M1_REGEN, "FlRegen");
        AT(M1_SEE_INVIS, "FlSeeInvis");
        AT(M1_TPORT, "FlTeleport");
        AT(M1_TPORT_CNTRL, "FlTeleportControl");
        AT(M1_ACID, "FlAcid");
        AT(M1_POIS, "FlPoisonous");
        AT(M1_CARNIVORE, "FlCarnivore");
        AT(M1_HERBIVORE, "FlHerbivore");
        AT(M1_METALLIVORE, "FlMetallivore");
#undef AT
#define AT(a, b) if (pm->mflags2 & a) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", b); \
}
        if (!polyok(pm)) {
            if ( comma_set ) fprintf(f, ", ");
            comma_set = 1;
            fprintf(f, "FlNoPoly");
        }
        if (touch_petrifies(pm)) {
            if ( comma_set ) fprintf(f, ", ");
            comma_set = 1;
            fprintf(f, "FlTouchPetrifies");
        }
        if (pm_invisible(pm)) {
            if ( comma_set ) fprintf(f, ", ");
            comma_set = 1;
            fprintf(f, "FlInvisible");
        }
        AT(M2_UNDEAD, "FlUndead");
        AT(M2_WERE, "FlWere");
        AT(M2_HUMAN, "FlHuman");
        AT(M2_ELF, "FlElf");
        AT(M2_DWARF, "FlDwarf");
        AT(M2_GNOME, "FlGnome");
        AT(M2_ORC, "FlOrc");
        AT(M2_DEMON, "FlDemon");
        AT(M2_MERC, "FlMerc");
        AT(M2_LORD, "FlLord");
        AT(M2_PRINCE, "FlPrince");
        AT(M2_MINION, "FlMinion");
        AT(M2_GIANT, "FlGiant");
        AT(M2_MALE, "FlMale");
        AT(M2_FEMALE, "FlFemale");
        AT(M2_NEUTER, "FlNeuter");
        AT(M2_PNAME, "FlProperName");
        AT(M2_HOSTILE, "FlHostile");
        AT(M2_PEACEFUL, "FlPeaceful");
        AT(M2_DOMESTIC, "FlDomestic");
        AT(M2_WANDER, "FlWander");
        AT(M2_STALK, "FlStalk");
        AT(M2_NASTY, "FlNasty");
        AT(M2_STRONG, "FlStrong");
        AT(M2_ROCKTHROW, "FlRockThrow");
        AT(M2_GREEDY, "FlGreedy");
        AT(M2_JEWELS, "FlJewels");
        AT(M2_COLLECT, "FlCollect");
        AT(M2_MAGIC, "FlMagicCollect");
#undef AT
        if (passes_walls(pm)) {
            fprintf(f, ", FlPhasing");
        }
#define AT(a, b) if (pm->mflags3 & a) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", b); \
}
        AT(M3_WANTSAMUL, "FlWantsAmulet");
        AT(M3_WANTSBELL, "FlWantsBell");
        AT(M3_WANTSBOOK, "FlWantsBook");
        AT(M3_WANTSCAND, "FlWantsCand");
        AT(M3_WANTSARTI, "FlWantsArti");
        AT(M3_WANTSALL, "FlWantsAll");
        AT(M3_WAITFORU, "FlWaitsForYou");
        AT(M3_CLOSE, "FlClose");
        AT(M3_COVETOUS, "FlCovetous");
#ifdef M3_LITHIVORE
        AT(M3_LITHIVORE, "FlLithivore");
#endif
#ifdef M3_POKEMON
        AT(M3_POKEMON, "FlPokemon");
#endif
#ifdef M3_AVOIDER
        AT(M3_AVOIDER, "FlAvoider");
#endif
#ifdef M3_NOTAME
        AT(M3_NOTAME, "FlUntameable");
#endif
#ifdef M3_TRAITOR
        AT(M3_TRAITOR, "FlTraitor");
#endif
#ifdef M3_INFRAVISIBLE
        AT(M3_INFRAVISIBLE, "FlInfravisible");
#endif
#ifdef M3_INFRAVISION
        AT(M3_INFRAVISION, "FlInfravision");
#endif
        if (hates_silver(pm)) fprintf(f, ", FlHatesSilver");
        if (passes_bars(pm)) {
            if ( comma_set ) fprintf(f, ", ");
            comma_set = 1;
            fprintf(f, "FlPassesBars");
        }
        if (vegan(pm)) fprintf(f, ", FlVegan");
        else if (vegetarian(pm)) fprintf(f, ", FlVegetarian");

#undef AT
#endif /*non DNETHACK*/

#ifdef DNETHACK
#define AT(a, b) if (pm->mflagsm & a) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", b); \
}
        AT(MM_FLY, "FlFly");
        AT(MM_SWIM, "FlSwim");
        AT(MM_AMORPHOUS, "FlAmorphous");
        AT(MM_WALLWALK, "FlWallwalk");
        AT(MM_CLING, "FlCling");
        AT(MM_TUNNEL, "FlTunnel");
        AT(MM_NEEDPICK, "FlNeedPick");
        AT(MM_AMPHIBIOUS, "FlAmphibious");
        AT(MM_BREATHLESS, "FlBreathless");
        AT(MM_TPORT, "FlTeleport");
        AT(MM_TPORT_CNTRL, "FlTeleportControl");
        AT(MM_TENGTPORT, "FlFastTeleport");
        AT(MM_STATIONARY, "FlStationary");
        AT(MM_FLOAT, "FlFloat");
        AT(MM_NOTONL, "FlNoLine");
        AT(MM_FLEETFLEE, "FlFlee");
#undef AT
#define AT(a, b) if (pm->mflagst & a) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", b); \
}
        AT(MT_CONCEAL, "FlConceal");
        AT(MT_HIDE, "FlHide");
        AT(MT_MINDLESS, "FlMindless");
        AT(MT_ANIMAL, "FlAnimalMind");
        AT(MT_CARNIVORE, "FlCarnivore");
        AT(MT_HERBIVORE, "FlHerbivore");
        AT(MT_HOSTILE, "FlHostile");
        AT(MT_PEACEFUL, "FlPeaceful");
        AT(MT_DOMESTIC, "FlDomestic");
        AT(MT_WANDER, "FlWander");
        AT(MT_STALK, "FlStalk");
        AT(MT_ROCKTHROW, "FlRockThrow");
        AT(MT_GREEDY, "FlGreedy");
        AT(MT_JEWELS, "FlJewels");
        AT(MT_COLLECT, "FlCollect");
        AT(MT_MAGIC, "FlMagicCollect");
        AT(MT_WANTSAMUL, "FlWantsAmulet");
        AT(MT_WANTSBELL, "FlWantsBell");
        AT(MT_WANTSBOOK, "FlWantsBook");
        AT(MT_WANTSCAND, "FlWantsCand");
        AT(MT_WANTSARTI, "FlWantsArti");
        AT(MT_WANTSALL, "FlWantsAll");
        AT(MT_COVETOUS, "FlCovetous");
        AT(MT_WAITFORU, "FlWaitsForYou");
        AT(MT_CLOSE, "FlClose");
        AT(MT_TRAITOR, "FlTraitor");
        AT(MT_NOTAKE, "FlNoTake");
        AT(MT_METALLIVORE, "FlMetallivore");
        AT(MT_MAGIVORE, "FlMagivore");
        AT(MT_BOLD, "FlBold");
#undef AT
#define AT(a, b) if (pm->mflagsb & a) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", b); \
}
        AT(MB_NOEYES, "FlNoEyes");
        AT(MB_NOHANDS, "FlNoHands");
        AT(MB_NOLIMBS, "FlNoLimbs");
        AT(MB_NOHEAD, "FlNoHead");
        AT(MB_UNSOLID, "FlUnSolid");
        AT(MB_THICK_HIDE, "FlThickHide");
        AT(MB_OVIPAROUS, "FlOviparous");
        AT(MB_ACID, "FlAcid");
        AT(MB_POIS, "FlPoisonous");
        AT(MB_CHILL, "FlChill");
        AT(MB_TOSTY, "FlHot");
        AT(MB_MALE, "FlMale");
        AT(MB_FEMALE, "FlFemale");
        AT(MB_NEUTER, "FlNeuter");
        AT(MB_STRONG, "FlStrong");
        AT(MB_WINGS, "FlWings");
        AT(MB_LONGHEAD, "FlAnimalHead");
        AT(MB_LONGNECK, "FlLongNeck");
#undef AT
if ((pm->mflagsb & (MB_HUMANOID|MB_ANIMAL)) == (MB_HUMANOID|MB_ANIMAL)) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", "FlCentauroid"); \
} else if ((pm->mflagsb & (MB_HUMANOID|MB_SLITHY)) == (MB_HUMANOID|MB_SLITHY)) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", "FlSnakeleg"); \
} else if ((pm->mflagsb & (MB_ANIMAL|MB_SLITHY)) == (MB_ANIMAL|MB_SLITHY)) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", "FlSnakeback"); \
} else if (pm->mflagsb & (MB_HUMANOID)) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", "FlHumanoid"); \
} else if (pm->mflagsb & (MB_ANIMAL)) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", "FlAnimalBody"); \
} else if (pm->mflagsb & (MB_SLITHY)) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", "FlSlithy"); \
}
#define AT(a, b) if (pm->mflagsv & a) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", b); \
}
        AT(MV_NORMAL, "FlNormalSight");
        AT(MV_INFRAVISION, "FlInfravision");
        AT(MV_DARKSIGHT, "FlDarkSight");
        AT(MV_LOWLIGHT2, "FlLowLight2");
        AT(MV_LOWLIGHT3, "FlLowLight3");
        AT(MV_CATSIGHT, "FlCatsight");
        AT(MV_ECHOLOCATE, "FlEcholocate");
        AT(MV_BLOODSENSE, "FlBloodsense");
        AT(MV_LIFESENSE, "FlLifesense");
        AT(MV_EXTRAMISSION, "FlExtramission");
        AT(MV_TELEPATHIC, "FlTelepathic");
        AT(MV_RLYEHIAN, "FlRlyehianSight");
        AT(MV_SEE_INVIS, "FlSeeInvis");
        AT(MV_DETECTION, "FlDetection");
        AT(MV_OMNI, "FlOmniscient");
        AT(MV_SCENT, "FlScent");
        AT(MV_EARTHSENSE, "FlEarthSense");
#undef AT
#define AT(a, b) if (pm->mflagsg & a) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", b); \
}
        AT(MG_REGEN, "FlRegen");
        AT(MG_NOPOLY, "FlNoPoly");
        AT(MG_MERC, "FlMerc");
        AT(MG_PNAME, "FlProperName");
        AT(MG_LORD, "FlLord");
        AT(MG_PRINCE, "FlPrince");
        AT(MG_NASTY, "FlNasty");
        AT(MG_INFRAVISIBLE, "FlInfravisible");
        AT(MG_OPAQUE, "FlOpaque");
        AT(MG_DISPLACEMENT, "FlDisplacement");
        AT(MG_HATESSILVER, "FlHatesSilver");
        AT(MG_HATESIRON, "FlFlHatesIron");
        AT(MG_HATESUNHOLY, "FlFlHatesCursed");
        AT(MG_RIDER, "FlRiderRes");
        AT(MG_DEADLY, "FlDeadly");
        AT(MG_TRACKER, "FlTracker");
        AT(MG_NOSPELLCOOLDOWN, "FlNoSpellCooldowns");
#undef AT
if ((pm->mflagsg & MG_WRESIST) == MG_WRESIST) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", "FlMeleeResist"); \
} else if ((pm->mflagsg & MG_RALL) == MG_RALL) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", "FlResAll"); \
} else if ((pm->mflagsg & MG_VPIERCE) == MG_VPIERCE) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", "FlVulPierce"); \
} else if ((pm->mflagsg & MG_VSLASH) == MG_VSLASH) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", "FlVulSlash"); \
} else if ((pm->mflagsg & MG_VBLUNT) == MG_VBLUNT) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", "FlVulBlunt"); \
} else if ((pm->mflagsg & MG_RPIERCE) == MG_RPIERCE) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", "FlResPierce"); \
} else if ((pm->mflagsg & MG_RSLASH) == MG_RSLASH) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", "FlResSlash"); \
} else if ((pm->mflagsg & MG_RBLUNT) == MG_RBLUNT) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", "FlResBlunt"); \
}
#define AT(a, b) if (pm->mflagsa & a) { \
    if ( comma_set ) fprintf(f, ", "); \
    comma_set = 1;\
    fprintf(f, "%s", b); \
}
        AT(MA_UNDEAD, "FlUndead");
        AT(MA_WERE, "FlWere");
        AT(MA_HUMAN, "FlHuman");
        AT(MA_ELF, "FlElf");
        AT(MA_DROW, "FlDrow");
        AT(MA_DWARF, "FlDwarf");
        AT(MA_GNOME, "FlGnome");
        AT(MA_ORC, "FlOrc");
        AT(MA_VAMPIRE, "FlVampire");
        AT(MA_CLOCK, "FlClockwork");
        AT(MA_UNLIVING, "FlUnliving");
        AT(MA_PLANT, "FlPlant");
        AT(MA_GIANT, "FlGiant");
        AT(MA_INSECTOID, "FlInsectiod");
        AT(MA_ARACHNID, "FlArachnid");
        AT(MA_AVIAN, "FlAvian");
        AT(MA_REPTILIAN, "FlReptilian");
        AT(MA_ANIMAL, "FlAnimal");
        AT(MA_AQUATIC, "FlAquatic");
        AT(MA_DEMIHUMAN, "FlDemihuman");
        AT(MA_FEY, "FlFey");
        AT(MA_ELEMENTAL, "FlElemental");
        AT(MA_DRAGON, "FlDragon");
        AT(MA_DEMON, "FlDemon");
        AT(MA_MINION, "FlMinion");
        AT(MA_PRIMORDIAL, "FlPrimordial");
        AT(MA_ET, "FlET");
#undef AT
        if (passes_bars(pm)) {
            if ( comma_set ) fprintf(f, ", ");
            comma_set = 1;
            fprintf(f, "FlPassesBars");
        }
        if (vegan(pm)) fprintf(f, ", FlVegan");
        else if (vegetarian(pm)) fprintf(f, ", FlVegetarian");

#endif /*DNETHACK*/
        }
        fprintf(f, "]\n");
        fprintf(f, "   color: ");
        switch(pm->mcolor)
        {
            case CLR_BLACK: fprintf(f, "Black"); break;
            case CLR_RED: fprintf(f, "Red"); break;
            case CLR_GREEN: fprintf(f, "Green"); break;
            case CLR_BROWN: fprintf(f, "Brown"); break;
            case CLR_BLUE: fprintf(f, "Blue"); break;
            case CLR_MAGENTA: fprintf(f, "Magenta"); break;
            case CLR_CYAN: fprintf(f, "Cyan"); break;
            case CLR_GRAY: fprintf(f, "Gray"); break;
            case CLR_ORANGE: fprintf(f, "Orange"); break;
            case CLR_BRIGHT_GREEN: fprintf(f, "BrightGreen"); break;
            case CLR_BRIGHT_BLUE: fprintf(f, "BrightBlue"); break;
            case CLR_BRIGHT_CYAN: fprintf(f, "BrightCyan"); break;
            case CLR_BRIGHT_MAGENTA: fprintf(f, "BrightMagenta"); break;
            case CLR_YELLOW: fprintf(f, "Yellow"); break;
            case CLR_WHITE: fprintf(f, "White"); break;
            default: fprintf(stderr, "I don't know what color %d is.\n",
                             pm->mcolor);
                     abort();
        }
        fprintf(f, "\n");
    }
    fprintf(f, "all-monster-names: [");
    for (i1 = 0; mons[i1].mname[0]; ++i1)
    {
        if (i1 > 0)
            fprintf(f, ", ");
        fprintf(f, "\"%s\"", mons[i1].mname);
    }
    fprintf(f, "]\n\n");
}


#ifdef HASSHEOL
static boolean pb_prohibited_by_generation_flags(struct permonst *ptr
        , int inhell, int insheol)
{
	if (inhell && !insheol) {
		/* In Gehennon, outside of the Sheol */
		if (ptr->geno & G_HELL) {
			return FALSE;
		}
		if (ptr->geno & G_NOHELL) {
			return TRUE;
		}
		if (ptr->geno & G_SHEOL) {
			return TRUE;
		}
		return FALSE;
	} else if (insheol) {
		/* In Sheol */
		if (ptr->geno & G_SHEOL) {
			return FALSE;
		}
		if (ptr->geno & G_NOSHEOL) {
			return TRUE;
		}
		if (ptr->geno & G_HELL) {
			return TRUE;
		}
		return FALSE;
	} else {
		/* Outside of Gehennon and Sheol*/
		if (ptr->geno & G_SHEOL) {
			return TRUE;
		}
		if (ptr->geno & G_HELL) {
			return TRUE;
		}
		return FALSE;
	}
}
#endif

/*unixmain.c*/
