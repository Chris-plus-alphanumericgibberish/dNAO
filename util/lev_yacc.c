/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     CHAR = 258,
     INTEGER = 259,
     BOOLEAN = 260,
     PERCENT = 261,
     MESSAGE_ID = 262,
     MAZE_ID = 263,
     LEVEL_ID = 264,
     LEV_INIT_ID = 265,
     GEOMETRY_ID = 266,
     NOMAP_ID = 267,
     OBJECT_ID = 268,
     COBJECT_ID = 269,
     MONSTER_ID = 270,
     TRAP_ID = 271,
     DOOR_ID = 272,
     DRAWBRIDGE_ID = 273,
     MAZEWALK_ID = 274,
     WALLIFY_ID = 275,
     REGION_ID = 276,
     FILLING = 277,
     RANDOM_OBJECTS_ID = 278,
     RANDOM_MONSTERS_ID = 279,
     RANDOM_PLACES_ID = 280,
     ALTAR_ID = 281,
     LADDER_ID = 282,
     STAIR_ID = 283,
     NON_DIGGABLE_ID = 284,
     NON_PASSWALL_ID = 285,
     ROOM_ID = 286,
     PORTAL_ID = 287,
     TELEPRT_ID = 288,
     BRANCH_ID = 289,
     LEV = 290,
     CHANCE_ID = 291,
     CORRIDOR_ID = 292,
     GOLD_ID = 293,
     ENGRAVING_ID = 294,
     FOUNTAIN_ID = 295,
     POOL_ID = 296,
     SINK_ID = 297,
     NONE = 298,
     RAND_CORRIDOR_ID = 299,
     DOOR_STATE = 300,
     LIGHT_STATE = 301,
     CURSE_TYPE = 302,
     ENGRAVING_TYPE = 303,
     DIRECTION = 304,
     RANDOM_TYPE = 305,
     O_REGISTER = 306,
     M_REGISTER = 307,
     P_REGISTER = 308,
     A_REGISTER = 309,
     ALIGNMENT = 310,
     LEFT_OR_RIGHT = 311,
     CENTER = 312,
     TOP_OR_BOT = 313,
     ALTAR_TYPE = 314,
     UP_OR_DOWN = 315,
     SUBROOM_ID = 316,
     NAME_ID = 317,
     FLAGS_ID = 318,
     FLAG_TYPE = 319,
     MON_ATTITUDE = 320,
     MON_ALERTNESS = 321,
     MON_APPEARANCE = 322,
     CONTAINED = 323,
     STRING = 324,
     MAP_ID = 325
   };
#endif
/* Tokens.  */
#define CHAR 258
#define INTEGER 259
#define BOOLEAN 260
#define PERCENT 261
#define MESSAGE_ID 262
#define MAZE_ID 263
#define LEVEL_ID 264
#define LEV_INIT_ID 265
#define GEOMETRY_ID 266
#define NOMAP_ID 267
#define OBJECT_ID 268
#define COBJECT_ID 269
#define MONSTER_ID 270
#define TRAP_ID 271
#define DOOR_ID 272
#define DRAWBRIDGE_ID 273
#define MAZEWALK_ID 274
#define WALLIFY_ID 275
#define REGION_ID 276
#define FILLING 277
#define RANDOM_OBJECTS_ID 278
#define RANDOM_MONSTERS_ID 279
#define RANDOM_PLACES_ID 280
#define ALTAR_ID 281
#define LADDER_ID 282
#define STAIR_ID 283
#define NON_DIGGABLE_ID 284
#define NON_PASSWALL_ID 285
#define ROOM_ID 286
#define PORTAL_ID 287
#define TELEPRT_ID 288
#define BRANCH_ID 289
#define LEV 290
#define CHANCE_ID 291
#define CORRIDOR_ID 292
#define GOLD_ID 293
#define ENGRAVING_ID 294
#define FOUNTAIN_ID 295
#define POOL_ID 296
#define SINK_ID 297
#define NONE 298
#define RAND_CORRIDOR_ID 299
#define DOOR_STATE 300
#define LIGHT_STATE 301
#define CURSE_TYPE 302
#define ENGRAVING_TYPE 303
#define DIRECTION 304
#define RANDOM_TYPE 305
#define O_REGISTER 306
#define M_REGISTER 307
#define P_REGISTER 308
#define A_REGISTER 309
#define ALIGNMENT 310
#define LEFT_OR_RIGHT 311
#define CENTER 312
#define TOP_OR_BOT 313
#define ALTAR_TYPE 314
#define UP_OR_DOWN 315
#define SUBROOM_ID 316
#define NAME_ID 317
#define FLAGS_ID 318
#define FLAG_TYPE 319
#define MON_ATTITUDE 320
#define MON_ALERTNESS 321
#define MON_APPEARANCE 322
#define CONTAINED 323
#define STRING 324
#define MAP_ID 325




/* Copy the first part of user declarations.  */
#line 1 "lev_comp.y"

/*	SCCS Id: @(#)lev_yacc.c	3.4	2000/01/17	*/
/*	Copyright (c) 1989 by Jean-Christophe Collet */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * This file contains the Level Compiler code
 * It may handle special mazes & special room-levels
 */

/* In case we're using bison in AIX.  This definition must be
 * placed before any other C-language construct in the file
 * excluding comments and preprocessor directives (thanks IBM
 * for this wonderful feature...).
 *
 * Note: some cpps barf on this 'undefined control' (#pragma).
 * Addition of the leading space seems to prevent barfage for now,
 * and AIX will still see the directive.
 */
#ifdef _AIX
 #pragma alloca		/* keep leading space! */
#endif

#include "hack.h"
#include "sp_lev.h"

#define MAX_REGISTERS	10
#define ERR		(-1)
/* many types of things are put in chars for transference to NetHack.
 * since some systems will use signed chars, limit everybody to the
 * same number for portability.
 */
#define MAX_OF_TYPE	128

#define New(type)		\
	(type *) memset((genericptr_t)alloc(sizeof(type)), 0, sizeof(type))
#define NewTab(type, size)	(type **) alloc(sizeof(type *) * size)
#define Free(ptr)		free((genericptr_t)ptr)

extern void FDECL(yyerror, (const char *));
extern void FDECL(yywarning, (const char *));
extern int NDECL(yylex);
int NDECL(yyparse);

extern int FDECL(get_floor_type, (CHAR_P));
extern int FDECL(get_room_type, (char *));
extern int FDECL(get_trap_type, (char *));
extern int FDECL(get_monster_id, (char *,CHAR_P));
extern int FDECL(get_object_id, (char *,CHAR_P));
extern boolean FDECL(check_monster_char, (CHAR_P));
extern boolean FDECL(check_object_char, (CHAR_P));
extern char FDECL(what_map_char, (CHAR_P));
extern void FDECL(scan_map, (char *));
extern void NDECL(wallify_map);
extern boolean NDECL(check_subrooms);
extern void FDECL(check_coord, (int,int,const char *));
extern void NDECL(store_part);
extern void NDECL(store_room);
extern boolean FDECL(write_level_file, (char *,splev *,specialmaze *));
extern void FDECL(free_rooms, (splev *));

static struct reg {
	int x1, y1;
	int x2, y2;
}		current_region;

static struct coord {
	int x;
	int y;
}		current_coord, current_align;

static struct size {
	int height;
	int width;
}		current_size;

char tmpmessage[256];
digpos *tmppass[32];
char *tmpmap[ROWNO];

digpos *tmpdig[MAX_OF_TYPE];
region *tmpreg[MAX_OF_TYPE];
lev_region *tmplreg[MAX_OF_TYPE];
door *tmpdoor[MAX_OF_TYPE];
drawbridge *tmpdb[MAX_OF_TYPE];
walk *tmpwalk[MAX_OF_TYPE];

room_door *tmprdoor[MAX_OF_TYPE];
trap *tmptrap[MAX_OF_TYPE];
monster *tmpmonst[MAX_OF_TYPE];
object *tmpobj[MAX_OF_TYPE];
altar *tmpaltar[MAX_OF_TYPE];
lad *tmplad[MAX_OF_TYPE];
stair *tmpstair[MAX_OF_TYPE];
gold *tmpgold[MAX_OF_TYPE];
engraving *tmpengraving[MAX_OF_TYPE];
fountain *tmpfountain[MAX_OF_TYPE];
sink *tmpsink[MAX_OF_TYPE];
pool *tmppool[MAX_OF_TYPE];

mazepart *tmppart[10];
room *tmproom[MAXNROFROOMS*2];
corridor *tmpcor[MAX_OF_TYPE];

static specialmaze maze;
static splev special_lev;
static lev_init init_lev;

static char olist[MAX_REGISTERS], mlist[MAX_REGISTERS];
static struct coord plist[MAX_REGISTERS];

int n_olist = 0, n_mlist = 0, n_plist = 0;

unsigned int nlreg = 0, nreg = 0, ndoor = 0, ntrap = 0, nmons = 0, nobj = 0;
unsigned int ndb = 0, nwalk = 0, npart = 0, ndig = 0, nlad = 0, nstair = 0;
unsigned int naltar = 0, ncorridor = 0, nrooms = 0, ngold = 0, nengraving = 0;
unsigned int nfountain = 0, npool = 0, nsink = 0, npass = 0;

static int lev_flags = 0;

unsigned int max_x_map, max_y_map;

static xchar in_room;

extern int fatal_error;
extern int want_warnings;
extern const char *fname;



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 132 "lev_comp.y"
{
	int	i;
	char*	map;
	struct {
		xchar room;
		xchar wall;
		xchar door;
	} corpos;
}
/* Line 187 of yacc.c.  */
#line 376 "y.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 389 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  12
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   448

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  77
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  119
/* YYNRULES -- Number of rules.  */
#define YYNRULES  223
/* YYNRULES -- Number of states.  */
#define YYNSTATES  476

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   325

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      71,    72,     2,     2,    69,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    70,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    73,     2,    74,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    75,    76
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     6,     8,    11,    13,    15,    21,
      29,    33,    34,    48,    50,    52,    53,    57,    61,    63,
      64,    67,    71,    72,    75,    79,    83,    84,    86,    88,
      91,    93,    95,    97,    98,   101,   107,   113,   121,   124,
     127,   140,   153,   154,   157,   163,   165,   171,   173,   179,
     181,   187,   189,   190,   193,   195,   197,   199,   201,   203,
     205,   207,   209,   211,   213,   215,   217,   219,   223,   227,
     237,   239,   241,   243,   245,   247,   249,   255,   257,   259,
     261,   264,   268,   270,   273,   279,   281,   283,   285,   287,
     288,   291,   295,   299,   303,   305,   309,   311,   315,   317,
     318,   323,   324,   327,   329,   331,   333,   335,   337,   339,
     341,   343,   345,   347,   349,   351,   353,   355,   357,   359,
     361,   363,   365,   367,   368,   379,   380,   383,   386,   389,
     392,   395,   399,   402,   405,   406,   416,   418,   420,   421,
     429,   435,   441,   443,   445,   447,   449,   451,   452,   455,
     458,   464,   471,   479,   485,   487,   493,   499,   500,   509,
     510,   519,   520,   521,   530,   531,   538,   539,   542,   544,
     555,   559,   563,   567,   571,   575,   584,   592,   598,   606,
     608,   610,   612,   614,   616,   618,   620,   622,   624,   626,
     628,   630,   632,   634,   635,   638,   643,   645,   647,   649,
     651,   653,   655,   657,   659,   661,   663,   665,   667,   672,
     677,   682,   687,   689,   691,   693,   695,   697,   699,   700,
     702,   704,   706,   712
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      78,     0,    -1,    -1,    79,    -1,    80,    -1,    80,    79,
      -1,    81,    -1,    82,    -1,   115,    86,    84,    88,   117,
      -1,    83,    86,    84,    88,    90,    92,    94,    -1,     9,
      70,   190,    -1,    -1,    10,    70,     3,    69,     3,    69,
       5,    69,     5,    69,   180,    69,    85,    -1,     5,    -1,
      50,    -1,    -1,    63,    70,    87,    -1,    64,    69,    87,
      -1,    64,    -1,    -1,    89,    88,    -1,     7,    70,    75,
      -1,    -1,    90,    91,    -1,    23,    70,   125,    -1,    24,
      70,   126,    -1,    -1,    93,    -1,    99,    -1,    99,    93,
      -1,    95,    -1,    96,    -1,    44,    -1,    -1,    96,    97,
      -1,    37,    70,    98,    69,    98,    -1,    37,    70,    98,
      69,     4,    -1,    71,     4,    69,    49,    69,   114,    72,
      -1,   101,   107,    -1,   100,   107,    -1,    61,    70,   176,
      69,   180,    69,   104,    69,   106,    69,   190,   102,    -1,
      31,    70,   176,    69,   180,    69,   103,    69,   105,    69,
     106,   102,    -1,    -1,    69,     5,    -1,    71,     4,    69,
       4,    72,    -1,    50,    -1,    71,     4,    69,     4,    72,
      -1,    50,    -1,    71,   121,    69,   122,    72,    -1,    50,
      -1,    71,     4,    69,     4,    72,    -1,    50,    -1,    -1,
     107,   108,    -1,   109,    -1,   110,    -1,   111,    -1,   131,
      -1,   135,    -1,   145,    -1,   168,    -1,   162,    -1,   163,
      -1,   164,    -1,   169,    -1,   170,    -1,   150,    -1,    62,
      70,   190,    -1,    36,    70,     4,    -1,    17,    70,   112,
      69,   179,    69,   113,    69,   114,    -1,     5,    -1,    50,
      -1,    49,    -1,    50,    -1,     4,    -1,    50,    -1,     8,
      70,   190,    69,   116,    -1,     3,    -1,    50,    -1,   118,
      -1,   118,   117,    -1,   119,   123,   129,    -1,    12,    -1,
     120,    76,    -1,    11,    70,   121,    69,   122,    -1,    56,
      -1,    57,    -1,    58,    -1,    57,    -1,    -1,   123,   124,
      -1,    23,    70,   125,    -1,    25,    70,   127,    -1,    24,
      70,   126,    -1,   189,    -1,   189,    69,   125,    -1,   188,
      -1,   188,    69,   126,    -1,   187,    -1,    -1,   187,   128,
      69,   127,    -1,    -1,   129,   130,    -1,   131,    -1,   135,
      -1,   144,    -1,   145,    -1,   146,    -1,   167,    -1,   151,
      -1,   153,    -1,   155,    -1,   158,    -1,   168,    -1,   162,
      -1,   147,    -1,   148,    -1,   149,    -1,   150,    -1,   169,
      -1,   170,    -1,   165,    -1,   166,    -1,    -1,    15,   192,
      70,   171,    69,   173,    69,   178,   132,   133,    -1,    -1,
     133,   134,    -1,    69,   190,    -1,    69,    65,    -1,    69,
      66,    -1,    69,   181,    -1,    69,    67,   190,    -1,    13,
     136,    -1,    14,   136,    -1,    -1,   192,    70,   172,    69,
     174,   137,    69,   138,   139,    -1,   178,    -1,    68,    -1,
      -1,    69,   140,    69,   141,    69,   142,   143,    -1,    69,
     140,    69,   142,   143,    -1,    69,   141,    69,   142,   143,
      -1,    50,    -1,    47,    -1,    75,    -1,    50,    -1,     4,
      -1,    -1,    69,    43,    -1,    69,    75,    -1,    17,    70,
     179,    69,   178,    -1,    16,   192,    70,   175,    69,   178,
      -1,    18,    70,   178,    69,    49,    69,   179,    -1,    19,
      70,   178,    69,    49,    -1,    20,    -1,    27,    70,   178,
      69,    60,    -1,    28,    70,   178,    69,    60,    -1,    -1,
      28,    70,   161,   152,    69,   161,    69,    60,    -1,    -1,
      32,    70,   161,   154,    69,   161,    69,   190,    -1,    -1,
      -1,    33,    70,   161,   156,    69,   161,   157,   160,    -1,
      -1,    34,    70,   161,   159,    69,   161,    -1,    -1,    69,
      60,    -1,   195,    -1,    35,    71,     4,    69,     4,    69,
       4,    69,     4,    72,    -1,    40,    70,   178,    -1,    42,
      70,   178,    -1,    41,    70,   178,    -1,    29,    70,   195,
      -1,    30,    70,   195,    -1,    21,    70,   195,    69,   180,
      69,   176,   177,    -1,    26,    70,   178,    69,   181,    69,
     182,    -1,    38,    70,   191,    69,   178,    -1,    39,    70,
     178,    69,   193,    69,   190,    -1,   188,    -1,    50,    -1,
     185,    -1,   189,    -1,    50,    -1,   184,    -1,   190,    -1,
      50,    -1,   190,    -1,    50,    -1,   190,    -1,    50,    -1,
     190,    -1,    50,    -1,    -1,    69,    22,    -1,    69,    22,
      69,     5,    -1,   194,    -1,   183,    -1,    50,    -1,    45,
      -1,    50,    -1,    46,    -1,    50,    -1,    55,    -1,   186,
      -1,    50,    -1,    59,    -1,    50,    -1,    53,    73,     4,
      74,    -1,    51,    73,     4,    74,    -1,    52,    73,     4,
      74,    -1,    54,    73,     4,    74,    -1,   194,    -1,     3,
      -1,     3,    -1,    75,    -1,     4,    -1,    50,    -1,    -1,
       6,    -1,    48,    -1,    50,    -1,    71,     4,    69,     4,
      72,    -1,    71,     4,    69,     4,    69,     4,    69,     4,
      72,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   169,   169,   170,   173,   174,   177,   178,   181,   208,
     245,   258,   266,   286,   287,   291,   294,   301,   305,   311,
     312,   315,   332,   333,   336,   347,   361,   375,   378,   379,
     382,   383,   386,   394,   395,   398,   413,   429,   439,   443,
     449,   467,   486,   489,   495,   505,   511,   520,   526,   531,
     537,   542,   548,   549,   552,   553,   554,   555,   556,   557,
     558,   559,   560,   561,   562,   563,   564,   567,   576,   589,
     609,   610,   613,   614,   617,   618,   621,   634,   638,   644,
     645,   648,   654,   670,   683,   689,   690,   693,   694,   697,
     698,   701,   712,   727,   740,   747,   756,   763,   772,   780,
     779,   789,   790,   793,   794,   795,   796,   797,   798,   799,
     800,   801,   802,   803,   804,   805,   806,   807,   808,   809,
     810,   811,   812,   816,   815,   851,   852,   855,   859,   863,
     867,   871,   878,   881,   891,   890,   918,   927,   937,   946,
     949,   952,   957,   961,   967,   978,   982,   988,   989,   992,
     998,  1016,  1033,  1075,  1089,  1095,  1112,  1130,  1129,  1159,
    1158,  1185,  1194,  1184,  1218,  1217,  1244,  1247,  1253,  1257,
    1277,  1293,  1306,  1319,  1334,  1349,  1415,  1433,  1450,  1468,
    1469,  1473,  1476,  1477,  1481,  1484,  1485,  1491,  1492,  1498,
    1506,  1509,  1519,  1523,  1526,  1530,  1536,  1537,  1538,  1544,
    1545,  1548,  1549,  1552,  1553,  1554,  1560,  1561,  1564,  1573,
    1582,  1591,  1600,  1603,  1614,  1626,  1629,  1630,  1634,  1637,
    1645,  1646,  1649,  1660
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "CHAR", "INTEGER", "BOOLEAN", "PERCENT",
  "MESSAGE_ID", "MAZE_ID", "LEVEL_ID", "LEV_INIT_ID", "GEOMETRY_ID",
  "NOMAP_ID", "OBJECT_ID", "COBJECT_ID", "MONSTER_ID", "TRAP_ID",
  "DOOR_ID", "DRAWBRIDGE_ID", "MAZEWALK_ID", "WALLIFY_ID", "REGION_ID",
  "FILLING", "RANDOM_OBJECTS_ID", "RANDOM_MONSTERS_ID", "RANDOM_PLACES_ID",
  "ALTAR_ID", "LADDER_ID", "STAIR_ID", "NON_DIGGABLE_ID",
  "NON_PASSWALL_ID", "ROOM_ID", "PORTAL_ID", "TELEPRT_ID", "BRANCH_ID",
  "LEV", "CHANCE_ID", "CORRIDOR_ID", "GOLD_ID", "ENGRAVING_ID",
  "FOUNTAIN_ID", "POOL_ID", "SINK_ID", "NONE", "RAND_CORRIDOR_ID",
  "DOOR_STATE", "LIGHT_STATE", "CURSE_TYPE", "ENGRAVING_TYPE", "DIRECTION",
  "RANDOM_TYPE", "O_REGISTER", "M_REGISTER", "P_REGISTER", "A_REGISTER",
  "ALIGNMENT", "LEFT_OR_RIGHT", "CENTER", "TOP_OR_BOT", "ALTAR_TYPE",
  "UP_OR_DOWN", "SUBROOM_ID", "NAME_ID", "FLAGS_ID", "FLAG_TYPE",
  "MON_ATTITUDE", "MON_ALERTNESS", "MON_APPEARANCE", "CONTAINED", "','",
  "':'", "'('", "')'", "'['", "']'", "STRING", "MAP_ID", "$accept", "file",
  "levels", "level", "maze_level", "room_level", "level_def", "lev_init",
  "walled", "flags", "flag_list", "messages", "message", "rreg_init",
  "init_rreg", "rooms", "roomlist", "corridors_def", "random_corridors",
  "corridors", "corridor", "corr_spec", "aroom", "subroom_def", "room_def",
  "roomfill", "room_pos", "subroom_pos", "room_align", "room_size",
  "room_details", "room_detail", "room_name", "room_chance", "room_door",
  "secret", "door_wall", "door_pos", "maze_def", "filling", "regions",
  "aregion", "map_definition", "map_geometry", "h_justif", "v_justif",
  "reg_init", "init_reg", "object_list", "monster_list", "place_list",
  "@1", "map_details", "map_detail", "monster_detail", "@2",
  "monster_infos", "monster_info", "object_detail", "object_desc", "@3",
  "object_where", "object_infos", "curse_state", "monster_id",
  "enchantment", "optional_name", "door_detail", "trap_detail",
  "drawbridge_detail", "mazewalk_detail", "wallify_detail",
  "ladder_detail", "stair_detail", "stair_region", "@4", "portal_region",
  "@5", "teleprt_region", "@6", "@7", "branch_region", "@8",
  "teleprt_detail", "lev_region", "fountain_detail", "sink_detail",
  "pool_detail", "diggable_detail", "passwall_detail", "region_detail",
  "altar_detail", "gold_detail", "engraving_detail", "monster_c",
  "object_c", "m_name", "o_name", "trap_name", "room_type", "prefilled",
  "coordinate", "door_state", "light_state", "alignment", "altar_type",
  "p_register", "o_register", "m_register", "a_register", "place",
  "monster", "object", "string", "amount", "chance", "engraving_type",
  "coord", "region", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,    44,
      58,    40,    41,    91,    93,   324,   325
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    77,    78,    78,    79,    79,    80,    80,    81,    82,
      83,    84,    84,    85,    85,    86,    86,    87,    87,    88,
      88,    89,    90,    90,    91,    91,    92,    92,    93,    93,
      94,    94,    95,    96,    96,    97,    97,    98,    99,    99,
     100,   101,   102,   102,   103,   103,   104,   104,   105,   105,
     106,   106,   107,   107,   108,   108,   108,   108,   108,   108,
     108,   108,   108,   108,   108,   108,   108,   109,   110,   111,
     112,   112,   113,   113,   114,   114,   115,   116,   116,   117,
     117,   118,   119,   119,   120,   121,   121,   122,   122,   123,
     123,   124,   124,   124,   125,   125,   126,   126,   127,   128,
     127,   129,   129,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   132,   131,   133,   133,   134,   134,   134,
     134,   134,   135,   135,   137,   136,   138,   138,   139,   139,
     139,   139,   140,   140,   141,   142,   142,   143,   143,   143,
     144,   145,   146,   147,   148,   149,   150,   152,   151,   154,
     153,   156,   157,   155,   159,   158,   160,   160,   161,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     171,   171,   172,   172,   172,   173,   173,   174,   174,   175,
     175,   176,   176,   177,   177,   177,   178,   178,   178,   179,
     179,   180,   180,   181,   181,   181,   182,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   191,   192,   192,
     193,   193,   194,   195
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     1,     2,     1,     1,     5,     7,
       3,     0,    13,     1,     1,     0,     3,     3,     1,     0,
       2,     3,     0,     2,     3,     3,     0,     1,     1,     2,
       1,     1,     1,     0,     2,     5,     5,     7,     2,     2,
      12,    12,     0,     2,     5,     1,     5,     1,     5,     1,
       5,     1,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     9,
       1,     1,     1,     1,     1,     1,     5,     1,     1,     1,
       2,     3,     1,     2,     5,     1,     1,     1,     1,     0,
       2,     3,     3,     3,     1,     3,     1,     3,     1,     0,
       4,     0,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,    10,     0,     2,     2,     2,     2,
       2,     3,     2,     2,     0,     9,     1,     1,     0,     7,
       5,     5,     1,     1,     1,     1,     1,     0,     2,     2,
       5,     6,     7,     5,     1,     5,     5,     0,     8,     0,
       8,     0,     0,     8,     0,     6,     0,     2,     1,    10,
       3,     3,     3,     3,     3,     8,     7,     5,     7,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     2,     4,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     4,     4,
       4,     4,     1,     1,     1,     1,     1,     1,     0,     1,
       1,     1,     5,     9
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     0,     0,     3,     4,     6,     7,    15,    15,
       0,     0,     1,     5,     0,    11,    11,   215,     0,    10,
       0,     0,    19,    19,     0,    18,    16,     0,     0,    22,
      19,     0,    77,    78,    76,     0,     0,     0,    26,    20,
       0,    82,     8,    79,    89,     0,    17,     0,    21,     0,
       0,     0,     0,    23,    33,    27,    28,    52,    52,     0,
      80,   101,    83,     0,     0,     0,     0,     0,    32,     9,
      30,    31,    29,    39,    38,    85,    86,     0,     0,     0,
       0,    90,    81,     0,   214,    24,    94,   213,    25,    96,
     192,     0,   191,     0,     0,    34,   218,   218,   218,   218,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      53,    54,    55,    56,    57,    58,    59,    66,    61,    62,
      63,    60,    64,    65,     0,     0,     0,     0,     0,     0,
       0,   154,     0,     0,     0,     0,     0,     0,     0,     0,
     102,   103,   104,   105,   106,   107,   115,   116,   117,   118,
     109,   110,   111,   112,   114,   121,   122,   108,   113,   119,
     120,     0,     0,     0,     0,     0,     0,   219,   132,     0,
     133,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    88,    87,    84,    91,    93,     0,    92,
      98,   212,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    95,    97,   201,   202,     0,     0,
       0,     0,     0,     0,     0,    70,    71,     0,   198,     0,
       0,   197,   196,     0,    68,   216,   217,     0,     0,   170,
     172,   171,    67,     0,     0,   199,   200,     0,     0,     0,
       0,     0,     0,     0,     0,   157,   168,   173,   174,   159,
     161,   164,     0,     0,     0,     0,     0,   183,     0,     0,
     184,   182,   180,     0,     0,   181,   179,   190,     0,   189,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    45,     0,     0,    47,     0,     0,     0,    36,    35,
       0,     0,     0,     0,     0,     0,     0,   205,     0,   203,
       0,   204,   156,   177,   220,   221,     0,     0,   100,   150,
       0,   153,     0,     0,   155,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   188,   134,
     187,     0,   186,     0,   185,   151,     0,   208,     0,     0,
       0,   222,     0,     0,     0,     0,     0,     0,     0,   162,
     165,     0,     0,    49,     0,     0,     0,    51,     0,     0,
       0,   209,     0,   210,     0,    72,    73,     0,     0,   207,
     206,   176,   178,   152,     0,   193,     0,     0,     0,   166,
      13,    14,    12,     0,     0,     0,     0,     0,     0,    74,
      75,     0,     0,   123,     0,   211,     0,     0,   175,     0,
     158,   160,     0,   163,    44,     0,    42,    46,     0,    42,
      37,   137,   138,   136,   125,    69,     0,   194,     0,   167,
       0,     0,    41,     0,    40,     0,   135,   124,     0,     0,
       0,    48,    43,    50,   143,   142,   144,     0,     0,     0,
     126,   223,   195,     0,     0,     0,   128,   129,     0,   130,
     127,   169,   146,   145,     0,   147,   147,   131,     0,     0,
     140,   141,   147,   148,   149,   139
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     3,     4,     5,     6,     7,     8,    22,   392,    15,
      26,    29,    30,    38,    53,    54,    55,    69,    70,    71,
      95,   211,    56,    57,    58,   432,   293,   296,   365,   369,
      73,   110,   111,   112,   113,   217,   377,   401,     9,    34,
      42,    43,    44,    45,    77,   185,    61,    81,    85,    88,
     189,   234,    82,   140,   114,   424,   437,   450,   115,   168,
     372,   422,   436,   447,   448,   465,   470,   143,   116,   145,
     146,   147,   148,   117,   150,   286,   151,   287,   152,   288,
     389,   153,   289,   413,   245,   118,   119,   120,   155,   156,
     157,   121,   122,   123,   264,   259,   343,   339,   268,    91,
     408,   223,   237,   208,   310,   381,   221,   260,   265,   311,
     190,    89,    86,    92,   227,   169,   316,   222,   246
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -416
static const yytype_int16 yypact[] =
{
     162,   -15,    34,   115,  -416,   162,  -416,  -416,    72,    72,
      38,    38,  -416,  -416,    90,   155,   155,  -416,   112,  -416,
     119,   114,   175,   175,    23,   120,  -416,   182,   121,  -416,
     175,   113,  -416,  -416,  -416,   119,   123,   111,    25,  -416,
     124,  -416,  -416,   113,  -416,   122,  -416,   184,  -416,   125,
     126,   127,   131,  -416,   144,  -416,     2,  -416,  -416,   117,
    -416,   128,  -416,   133,   187,   190,   -14,   -14,  -416,  -416,
    -416,   163,  -416,     3,     3,  -416,  -416,   136,   137,   138,
     139,  -416,   129,   194,  -416,  -416,   141,  -416,  -416,   142,
    -416,   143,  -416,   145,   146,  -416,   200,   200,   200,   200,
     147,   148,   149,   150,   151,   152,   153,   154,   156,   157,
    -416,  -416,  -416,  -416,  -416,  -416,  -416,  -416,  -416,  -416,
    -416,  -416,  -416,  -416,   118,   187,   190,   158,   160,   164,
     165,  -416,   166,   167,   168,   169,   170,   171,   172,   173,
    -416,  -416,  -416,  -416,  -416,  -416,  -416,  -416,  -416,  -416,
    -416,  -416,  -416,  -416,  -416,  -416,  -416,  -416,  -416,  -416,
    -416,   159,   187,   190,    55,    55,   161,  -416,  -416,   174,
    -416,   176,   177,    29,    41,    41,   209,    24,    41,    41,
      41,    41,    38,  -416,  -416,  -416,  -416,  -416,   211,  -416,
     179,  -416,    86,    41,    41,   178,    41,    16,   178,   178,
      12,    12,    12,   220,  -416,  -416,  -416,  -416,   181,   183,
     229,   185,    21,    18,   -13,  -416,  -416,   186,  -416,   180,
     188,  -416,  -416,   189,  -416,  -416,  -416,   191,   192,  -416,
    -416,  -416,  -416,   193,   195,  -416,  -416,   196,   197,   198,
     241,   199,   201,   202,   247,  -416,  -416,  -416,  -416,  -416,
    -416,  -416,   203,    -4,    45,   205,    11,  -416,   204,   206,
    -416,  -416,  -416,   207,   210,  -416,  -416,  -416,   212,  -416,
      86,   252,    73,   216,    41,   116,   255,   158,    41,   214,
     222,   213,    55,   218,   265,   215,   217,   219,   221,   223,
      55,  -416,   279,   225,  -416,   281,   226,   238,  -416,  -416,
     292,    27,   293,    31,    41,   230,   224,  -416,   227,  -416,
     232,  -416,  -416,  -416,  -416,  -416,   233,   231,  -416,  -416,
     235,  -416,   301,   237,  -416,   239,   303,    12,    12,    12,
      12,   240,   242,    49,   243,    50,   244,   236,  -416,  -416,
    -416,   245,  -416,   246,  -416,  -416,   130,  -416,   310,     0,
      38,  -416,    86,   248,   -14,   312,    65,   249,   251,  -416,
    -416,    30,   317,  -416,   117,   253,   319,  -416,   320,   256,
      26,  -416,   257,  -416,    41,  -416,  -416,   258,   254,  -416,
    -416,  -416,  -416,  -416,   325,   261,   262,   272,    38,   264,
    -416,  -416,  -416,   263,   267,    50,   266,   268,    38,  -416,
    -416,   269,    61,  -416,    26,  -416,   270,   321,  -416,   330,
    -416,  -416,   282,  -416,  -416,   118,   275,  -416,   341,   275,
    -416,  -416,   277,  -416,  -416,  -416,   343,   280,   283,  -416,
     276,   345,  -416,   284,  -416,    42,  -416,   285,   286,   346,
     349,  -416,  -416,  -416,  -416,  -416,  -416,   288,   290,    43,
    -416,  -416,  -416,   289,    10,    28,  -416,  -416,    38,  -416,
    -416,  -416,  -416,  -416,   291,   294,   294,  -416,    28,     9,
    -416,  -416,   294,  -416,  -416,  -416
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -416,  -416,   350,  -416,  -416,  -416,  -416,   348,  -416,   353,
     331,    96,  -416,  -416,  -416,  -416,   309,  -416,  -416,  -416,
    -416,   134,  -416,  -416,  -416,   -52,  -416,  -416,  -416,   -27,
     311,  -416,  -416,  -416,  -416,  -416,  -416,   -34,  -416,  -416,
     328,  -416,  -416,  -416,     8,   -42,  -416,  -416,   -98,   -88,
      97,  -416,  -416,  -416,   295,  -416,  -416,  -416,   297,   278,
    -416,  -416,  -416,  -416,   -78,  -415,  -376,  -416,   298,  -416,
    -416,  -416,  -416,   299,  -416,  -416,  -416,  -416,  -416,  -416,
    -416,  -416,  -416,  -416,  -189,   300,  -416,  -416,  -416,  -416,
    -416,   302,   304,   305,  -416,  -416,  -416,  -416,  -416,   -65,
    -416,  -171,  -264,  -160,   -66,  -416,  -416,  -416,  -416,  -416,
    -416,   208,   228,   -10,  -416,    79,  -416,  -123,  -141
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -100
static const yytype_int16 yytable[] =
{
      18,    19,    93,   220,   191,   209,   305,   228,   229,   230,
     231,   249,   250,   251,   462,   298,    96,    97,    98,    99,
     100,    87,   238,   239,    84,   242,    32,   186,   225,   101,
     399,   102,   462,    51,   215,   390,    90,   267,   187,   103,
     466,   104,   105,   106,   107,   108,   291,   243,    49,    50,
     379,   243,   473,   472,   241,    10,    51,   247,   248,   380,
     463,    17,    17,    52,   204,   109,   218,   292,   262,   219,
     263,   257,   258,    33,   226,   205,   400,   338,   463,   216,
     391,   342,   210,   240,   474,   446,    52,   244,   383,   444,
     471,   218,   445,   307,   219,   294,   475,   308,   309,   363,
     367,   206,    17,   313,    11,   207,    17,   319,   456,   457,
     458,   218,   188,    17,   219,    12,   295,   446,    17,    31,
     364,   368,   323,   307,    40,    41,    39,   308,   309,   421,
     331,   235,   188,   345,   384,    14,   236,   351,   357,   358,
     359,   360,    96,    97,    98,    99,   128,   129,   130,   131,
     132,    78,    79,    80,   191,   101,   133,   134,   135,   136,
      20,   137,   138,   139,   314,    21,   315,   104,   105,   106,
       1,     2,   232,    75,    76,   183,   184,   171,   172,   375,
     376,    24,    28,    25,    27,    36,    48,    63,    68,    35,
      84,    37,    47,    87,    59,    64,    65,    66,    62,   161,
      94,    67,    83,   403,   269,   124,   167,   125,   126,   127,
     162,   163,   164,   224,   165,   233,   166,   173,   174,   175,
     176,   177,   178,   179,   180,   252,   181,   182,   203,   188,
     192,   423,   210,   255,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   212,   281,   213,   214,   -99,   240,
     253,   285,   254,   271,   256,   270,   306,   272,   273,   317,
     274,   275,   276,   320,   277,   278,   279,   280,   282,   325,
     283,   321,   290,   284,   297,   301,   312,   300,   324,   303,
     302,   304,   322,   332,   326,   334,   327,   336,   328,   385,
     329,   340,   330,   344,   333,   335,   337,   341,   347,   346,
     348,   349,   350,   351,   352,   353,   354,   356,   355,   361,
     371,   362,   366,   370,   378,   374,   386,   384,   387,   373,
     388,   393,   395,   396,   397,   398,   402,   404,   405,   406,
     407,   409,   410,   412,   428,   414,   415,   418,   417,   426,
     382,   420,   429,   427,   431,   433,   435,   438,   441,   439,
     442,   452,   440,   453,   449,    13,   443,   454,   451,   455,
     468,   461,    16,   469,    23,    72,    46,   434,   416,    74,
     425,    60,   394,   430,   318,   170,   464,   141,   411,   142,
     144,   149,   154,   459,   158,     0,   159,   160,   419,     0,
     299,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   266,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   460,
     261,     0,     0,     0,     0,     0,     0,     0,   467
};

static const yytype_int16 yycheck[] =
{
      10,    11,    67,   174,   127,   165,   270,   178,   179,   180,
     181,   200,   201,   202,     4,     4,    13,    14,    15,    16,
      17,     3,   193,   194,     3,   196,     3,   125,     4,    26,
       4,    28,     4,    31,     5,     5,    50,    50,   126,    36,
     455,    38,    39,    40,    41,    42,    50,    35,    23,    24,
      50,    35,    43,   468,   195,    70,    31,   198,   199,    59,
      50,    75,    75,    61,   162,    62,    50,    71,    50,    53,
      52,    50,    51,    50,    50,   163,    50,    50,    50,    50,
      50,    50,    71,    71,    75,    75,    61,    71,   352,    47,
     466,    50,    50,    50,    53,    50,   472,    54,    55,    50,
      50,    46,    75,   274,    70,    50,    75,   278,    65,    66,
      67,    50,    71,    75,    53,     0,    71,    75,    75,    23,
      71,    71,   282,    50,    11,    12,    30,    54,    55,    68,
     290,    45,    71,   304,    69,    63,    50,    72,   327,   328,
     329,   330,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    23,    24,    25,   277,    26,    27,    28,    29,    30,
      70,    32,    33,    34,    48,    10,    50,    38,    39,    40,
       8,     9,   182,    56,    57,    57,    58,    98,    99,    49,
      50,    69,     7,    64,    70,     3,    75,     3,    44,    69,
       3,    70,    69,     3,    70,    70,    70,    70,    76,     5,
      37,    70,    69,   374,   214,    69,     6,    70,    70,    70,
      69,    69,    69,     4,    69,     4,    70,    70,    70,    70,
      70,    70,    70,    70,    70,     5,    70,    70,    69,    71,
      70,   402,    71,     4,    70,    70,    70,    70,    70,    70,
      70,    70,    70,    70,    70,     4,    70,    70,    69,    71,
      69,     4,    69,    73,    69,    69,     4,    69,    69,     4,
      69,    69,    69,    49,    69,    69,    69,    69,    69,     4,
      69,    49,    69,    71,    69,    69,    60,    73,    60,    69,
      73,    69,    69,     4,    69,     4,    69,    49,    69,   354,
      69,   301,    69,   303,    69,    69,     4,     4,    74,    69,
      73,    69,    69,    72,    69,     4,    69,     4,    69,    69,
      74,    69,    69,    69,     4,    69,     4,    69,    69,    74,
      69,     4,    69,     4,     4,    69,    69,    69,    74,     4,
      69,    69,    60,    69,     4,    72,    69,    69,    72,    69,
     350,    72,    60,    22,    69,     4,    69,     4,    72,    69,
       5,     5,    69,     4,    69,     5,    72,    69,    72,    69,
      69,    72,     9,    69,    16,    56,    35,   419,   395,    58,
     404,    43,   364,   415,   277,    97,   454,    82,   388,    82,
      82,    82,    82,   449,    82,    -1,    82,    82,   398,    -1,
     256,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   213,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   449,
     212,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   458
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     8,     9,    78,    79,    80,    81,    82,    83,   115,
      70,    70,     0,    79,    63,    86,    86,    75,   190,   190,
      70,    10,    84,    84,    69,    64,    87,    70,     7,    88,
      89,    88,     3,    50,   116,    69,     3,    70,    90,    88,
      11,    12,   117,   118,   119,   120,    87,    69,    75,    23,
      24,    31,    61,    91,    92,    93,    99,   100,   101,    70,
     117,   123,    76,     3,    70,    70,    70,    70,    44,    94,
      95,    96,    93,   107,   107,    56,    57,   121,    23,    24,
      25,   124,   129,    69,     3,   125,   189,     3,   126,   188,
      50,   176,   190,   176,    37,    97,    13,    14,    15,    16,
      17,    26,    28,    36,    38,    39,    40,    41,    42,    62,
     108,   109,   110,   111,   131,   135,   145,   150,   162,   163,
     164,   168,   169,   170,    69,    70,    70,    70,    17,    18,
      19,    20,    21,    27,    28,    29,    30,    32,    33,    34,
     130,   131,   135,   144,   145,   146,   147,   148,   149,   150,
     151,   153,   155,   158,   162,   165,   166,   167,   168,   169,
     170,     5,    69,    69,    69,    69,    70,     6,   136,   192,
     136,   192,   192,    70,    70,    70,    70,    70,    70,    70,
      70,    70,    70,    57,    58,   122,   125,   126,    71,   127,
     187,   194,    70,    70,    70,    70,    70,    70,    70,    70,
      70,    70,    70,    69,   125,   126,    46,    50,   180,   180,
      71,    98,    70,    70,    70,     5,    50,   112,    50,    53,
     178,   183,   194,   178,     4,     4,    50,   191,   178,   178,
     178,   178,   190,     4,   128,    45,    50,   179,   178,   178,
      71,   195,   178,    35,    71,   161,   195,   195,   195,   161,
     161,   161,     5,    69,    69,     4,    69,    50,    51,   172,
     184,   189,    50,    52,   171,   185,   188,    50,   175,   190,
      69,    73,    69,    69,    69,    69,    69,    69,    69,    69,
      69,     4,    69,    69,    71,     4,   152,   154,   156,   159,
      69,    50,    71,   103,    50,    71,   104,    69,     4,    98,
      73,    69,    73,    69,    69,   179,     4,    50,    54,    55,
     181,   186,    60,   178,    48,    50,   193,     4,   127,   178,
      49,    49,    69,   180,    60,     4,    69,    69,    69,    69,
      69,   180,     4,    69,     4,    69,    49,     4,    50,   174,
     190,     4,    50,   173,   190,   178,    69,    74,    73,    69,
      69,    72,    69,     4,    69,    69,     4,   161,   161,   161,
     161,    69,    69,    50,    71,   105,    69,    50,    71,   106,
      69,    74,   137,    74,    69,    49,    50,   113,     4,    50,
      59,   182,   190,   179,    69,   176,     4,    69,    69,   157,
       5,    50,    85,     4,   121,    69,     4,     4,    69,     4,
      50,   114,    69,   178,    69,    74,     4,    69,   177,    69,
      60,   190,    69,   160,    72,    69,   106,    72,    69,   190,
      72,    68,   138,   178,   132,   114,    69,    22,     4,    60,
     122,    69,   102,     4,   102,    69,   139,   133,     4,    69,
      69,    72,     5,    72,    47,    50,    75,   140,   141,    69,
     134,    72,     5,     4,    69,    69,    65,    66,    67,   181,
     190,    72,     4,    50,   141,   142,   142,   190,    69,    69,
     143,   143,   142,    43,    75,   143
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 8:
#line 182 "lev_comp.y"
    {
			unsigned i;

			if (fatal_error > 0) {
				(void) fprintf(stderr,
				"%s : %d errors detected. No output created!\n",
					fname, fatal_error);
			} else {
				maze.flags = (yyvsp[(2) - (5)].i);
				(void) memcpy((genericptr_t)&(maze.init_lev),
						(genericptr_t)&(init_lev),
						sizeof(lev_init));
				maze.numpart = npart;
				maze.parts = NewTab(mazepart, npart);
				for(i=0;i<npart;i++)
				    maze.parts[i] = tmppart[i];
				if (!write_level_file((yyvsp[(1) - (5)].map), (splev *)0, &maze)) {
					yyerror("Can't write output file!!");
					exit(EXIT_FAILURE);
				}
				npart = 0;
			}
			Free((yyvsp[(1) - (5)].map));
		  }
    break;

  case 9:
#line 209 "lev_comp.y"
    {
			unsigned i;

			if (fatal_error > 0) {
			    (void) fprintf(stderr,
			      "%s : %d errors detected. No output created!\n",
					fname, fatal_error);
			} else {
				special_lev.flags = (long) (yyvsp[(2) - (7)].i);
				(void) memcpy(
					(genericptr_t)&(special_lev.init_lev),
					(genericptr_t)&(init_lev),
					sizeof(lev_init));
				special_lev.nroom = nrooms;
				special_lev.rooms = NewTab(room, nrooms);
				for(i=0; i<nrooms; i++)
				    special_lev.rooms[i] = tmproom[i];
				special_lev.ncorr = ncorridor;
				special_lev.corrs = NewTab(corridor, ncorridor);
				for(i=0; i<ncorridor; i++)
				    special_lev.corrs[i] = tmpcor[i];
				if (check_subrooms()) {
				    if (!write_level_file((yyvsp[(1) - (7)].map), &special_lev,
							  (specialmaze *)0)) {
					yyerror("Can't write output file!!");
					exit(EXIT_FAILURE);
				    }
				}
				free_rooms(&special_lev);
				nrooms = 0;
				ncorridor = 0;
			}
			Free((yyvsp[(1) - (7)].map));
		  }
    break;

  case 10:
#line 246 "lev_comp.y"
    {
			if (index((yyvsp[(3) - (3)].map), '.'))
			    yyerror("Invalid dot ('.') in level name.");
			if ((int) strlen((yyvsp[(3) - (3)].map)) > 8)
			    yyerror("Level names limited to 8 characters.");
			(yyval.map) = (yyvsp[(3) - (3)].map);
			special_lev.nrmonst = special_lev.nrobjects = 0;
			n_mlist = n_olist = 0;
		  }
    break;

  case 11:
#line 258 "lev_comp.y"
    {
			/* in case we're processing multiple files,
			   explicitly clear any stale settings */
			(void) memset((genericptr_t) &init_lev, 0,
					sizeof init_lev);
			init_lev.init_present = FALSE;
			(yyval.i) = 0;
		  }
    break;

  case 12:
#line 267 "lev_comp.y"
    {
			init_lev.init_present = TRUE;
			init_lev.fg = what_map_char((char) (yyvsp[(3) - (13)].i));
			if (init_lev.fg == INVALID_TYPE)
			    yyerror("Invalid foreground type.");
			init_lev.bg = what_map_char((char) (yyvsp[(5) - (13)].i));
			if (init_lev.bg == INVALID_TYPE)
			    yyerror("Invalid background type.");
			init_lev.smoothed = (yyvsp[(7) - (13)].i);
			init_lev.joined = (yyvsp[(9) - (13)].i);
			if (init_lev.joined &&
			    init_lev.fg != CORR && init_lev.fg != ROOM)
			    yyerror("Invalid foreground type for joined map.");
			init_lev.lit = (yyvsp[(11) - (13)].i);
			init_lev.walled = (yyvsp[(13) - (13)].i);
			(yyval.i) = 1;
		  }
    break;

  case 15:
#line 291 "lev_comp.y"
    {
			(yyval.i) = 0;
		  }
    break;

  case 16:
#line 295 "lev_comp.y"
    {
			(yyval.i) = lev_flags;
			lev_flags = 0;	/* clear for next user */
		  }
    break;

  case 17:
#line 302 "lev_comp.y"
    {
			lev_flags |= (yyvsp[(1) - (3)].i);
		  }
    break;

  case 18:
#line 306 "lev_comp.y"
    {
			lev_flags |= (yyvsp[(1) - (1)].i);
		  }
    break;

  case 21:
#line 316 "lev_comp.y"
    {
			int i, j;

			i = (int) strlen((yyvsp[(3) - (3)].map)) + 1;
			j = (int) strlen(tmpmessage);
			if (i + j > 255) {
			   yyerror("Message string too long (>256 characters)");
			} else {
			    if (j) tmpmessage[j++] = '\n';
			    (void) strncpy(tmpmessage+j, (yyvsp[(3) - (3)].map), i - 1);
			    tmpmessage[j + i - 1] = 0;
			}
			Free((yyvsp[(3) - (3)].map));
		  }
    break;

  case 24:
#line 337 "lev_comp.y"
    {
			if(special_lev.nrobjects) {
			    yyerror("Object registers already initialized!");
			} else {
			    special_lev.nrobjects = n_olist;
			    special_lev.robjects = (char *) alloc(n_olist);
			    (void) memcpy((genericptr_t)special_lev.robjects,
					  (genericptr_t)olist, n_olist);
			}
		  }
    break;

  case 25:
#line 348 "lev_comp.y"
    {
			if(special_lev.nrmonst) {
			    yyerror("Monster registers already initialized!");
			} else {
			    special_lev.nrmonst = n_mlist;
			    special_lev.rmonst = (char *) alloc(n_mlist);
			    (void) memcpy((genericptr_t)special_lev.rmonst,
					  (genericptr_t)mlist, n_mlist);
			  }
		  }
    break;

  case 26:
#line 361 "lev_comp.y"
    {
			tmproom[nrooms] = New(room);
			tmproom[nrooms]->name = (char *) 0;
			tmproom[nrooms]->parent = (char *) 0;
			tmproom[nrooms]->rtype = 0;
			tmproom[nrooms]->rlit = 0;
			tmproom[nrooms]->xalign = ERR;
			tmproom[nrooms]->yalign = ERR;
			tmproom[nrooms]->x = 0;
			tmproom[nrooms]->y = 0;
			tmproom[nrooms]->w = 2;
			tmproom[nrooms]->h = 2;
			in_room = 1;
		  }
    break;

  case 32:
#line 387 "lev_comp.y"
    {
			tmpcor[0] = New(corridor);
			tmpcor[0]->src.room = -1;
			ncorridor = 1;
		  }
    break;

  case 35:
#line 399 "lev_comp.y"
    {
			tmpcor[ncorridor] = New(corridor);
			tmpcor[ncorridor]->src.room = (yyvsp[(3) - (5)].corpos).room;
			tmpcor[ncorridor]->src.wall = (yyvsp[(3) - (5)].corpos).wall;
			tmpcor[ncorridor]->src.door = (yyvsp[(3) - (5)].corpos).door;
			tmpcor[ncorridor]->dest.room = (yyvsp[(5) - (5)].corpos).room;
			tmpcor[ncorridor]->dest.wall = (yyvsp[(5) - (5)].corpos).wall;
			tmpcor[ncorridor]->dest.door = (yyvsp[(5) - (5)].corpos).door;
			ncorridor++;
			if (ncorridor >= MAX_OF_TYPE) {
				yyerror("Too many corridors in level!");
				ncorridor--;
			}
		  }
    break;

  case 36:
#line 414 "lev_comp.y"
    {
			tmpcor[ncorridor] = New(corridor);
			tmpcor[ncorridor]->src.room = (yyvsp[(3) - (5)].corpos).room;
			tmpcor[ncorridor]->src.wall = (yyvsp[(3) - (5)].corpos).wall;
			tmpcor[ncorridor]->src.door = (yyvsp[(3) - (5)].corpos).door;
			tmpcor[ncorridor]->dest.room = -1;
			tmpcor[ncorridor]->dest.wall = (yyvsp[(5) - (5)].i);
			ncorridor++;
			if (ncorridor >= MAX_OF_TYPE) {
				yyerror("Too many corridors in level!");
				ncorridor--;
			}
		  }
    break;

  case 37:
#line 430 "lev_comp.y"
    {
			if ((unsigned) (yyvsp[(2) - (7)].i) >= nrooms)
			    yyerror("Wrong room number!");
			(yyval.corpos).room = (yyvsp[(2) - (7)].i);
			(yyval.corpos).wall = (yyvsp[(4) - (7)].i);
			(yyval.corpos).door = (yyvsp[(6) - (7)].i);
		  }
    break;

  case 38:
#line 440 "lev_comp.y"
    {
			store_room();
		  }
    break;

  case 39:
#line 444 "lev_comp.y"
    {
			store_room();
		  }
    break;

  case 40:
#line 450 "lev_comp.y"
    {
			tmproom[nrooms] = New(room);
			tmproom[nrooms]->parent = (yyvsp[(11) - (12)].map);
			tmproom[nrooms]->name = (char *) 0;
			tmproom[nrooms]->rtype = (yyvsp[(3) - (12)].i);
			tmproom[nrooms]->rlit = (yyvsp[(5) - (12)].i);
			tmproom[nrooms]->filled = (yyvsp[(12) - (12)].i);
			tmproom[nrooms]->xalign = ERR;
			tmproom[nrooms]->yalign = ERR;
			tmproom[nrooms]->x = current_coord.x;
			tmproom[nrooms]->y = current_coord.y;
			tmproom[nrooms]->w = current_size.width;
			tmproom[nrooms]->h = current_size.height;
			in_room = 1;
		  }
    break;

  case 41:
#line 468 "lev_comp.y"
    {
			tmproom[nrooms] = New(room);
			tmproom[nrooms]->name = (char *) 0;
			tmproom[nrooms]->parent = (char *) 0;
			tmproom[nrooms]->rtype = (yyvsp[(3) - (12)].i);
			tmproom[nrooms]->rlit = (yyvsp[(5) - (12)].i);
			tmproom[nrooms]->filled = (yyvsp[(12) - (12)].i);
			tmproom[nrooms]->xalign = current_align.x;
			tmproom[nrooms]->yalign = current_align.y;
			tmproom[nrooms]->x = current_coord.x;
			tmproom[nrooms]->y = current_coord.y;
			tmproom[nrooms]->w = current_size.width;
			tmproom[nrooms]->h = current_size.height;
			in_room = 1;
		  }
    break;

  case 42:
#line 486 "lev_comp.y"
    {
			(yyval.i) = 1;
		  }
    break;

  case 43:
#line 490 "lev_comp.y"
    {
			(yyval.i) = (yyvsp[(2) - (2)].i);
		  }
    break;

  case 44:
#line 496 "lev_comp.y"
    {
			if ( (yyvsp[(2) - (5)].i) < 1 || (yyvsp[(2) - (5)].i) > 5 ||
			    (yyvsp[(4) - (5)].i) < 1 || (yyvsp[(4) - (5)].i) > 5 ) {
			    yyerror("Room position should be between 1 & 5!");
			} else {
			    current_coord.x = (yyvsp[(2) - (5)].i);
			    current_coord.y = (yyvsp[(4) - (5)].i);
			}
		  }
    break;

  case 45:
#line 506 "lev_comp.y"
    {
			current_coord.x = current_coord.y = ERR;
		  }
    break;

  case 46:
#line 512 "lev_comp.y"
    {
			if ( (yyvsp[(2) - (5)].i) < 0 || (yyvsp[(4) - (5)].i) < 0) {
			    yyerror("Invalid subroom position !");
			} else {
			    current_coord.x = (yyvsp[(2) - (5)].i);
			    current_coord.y = (yyvsp[(4) - (5)].i);
			}
		  }
    break;

  case 47:
#line 521 "lev_comp.y"
    {
			current_coord.x = current_coord.y = ERR;
		  }
    break;

  case 48:
#line 527 "lev_comp.y"
    {
			current_align.x = (yyvsp[(2) - (5)].i);
			current_align.y = (yyvsp[(4) - (5)].i);
		  }
    break;

  case 49:
#line 532 "lev_comp.y"
    {
			current_align.x = current_align.y = ERR;
		  }
    break;

  case 50:
#line 538 "lev_comp.y"
    {
			current_size.width = (yyvsp[(2) - (5)].i);
			current_size.height = (yyvsp[(4) - (5)].i);
		  }
    break;

  case 51:
#line 543 "lev_comp.y"
    {
			current_size.height = current_size.width = ERR;
		  }
    break;

  case 67:
#line 568 "lev_comp.y"
    {
			if (tmproom[nrooms]->name)
			    yyerror("This room already has a name!");
			else
			    tmproom[nrooms]->name = (yyvsp[(3) - (3)].map);
		  }
    break;

  case 68:
#line 577 "lev_comp.y"
    {
			if (tmproom[nrooms]->chance)
			    yyerror("This room already assigned a chance!");
			else if (tmproom[nrooms]->rtype == OROOM)
			    yyerror("Only typed rooms can have a chance!");
			else if ((yyvsp[(3) - (3)].i) < 1 || (yyvsp[(3) - (3)].i) > 99)
			    yyerror("The chance is supposed to be percentile.");
			else
			    tmproom[nrooms]->chance = (yyvsp[(3) - (3)].i);
		   }
    break;

  case 69:
#line 590 "lev_comp.y"
    {
			/* ERR means random here */
			if ((yyvsp[(7) - (9)].i) == ERR && (yyvsp[(9) - (9)].i) != ERR) {
		     yyerror("If the door wall is random, so must be its pos!");
			} else {
			    tmprdoor[ndoor] = New(room_door);
			    tmprdoor[ndoor]->secret = (yyvsp[(3) - (9)].i);
			    tmprdoor[ndoor]->mask = (yyvsp[(5) - (9)].i);
			    tmprdoor[ndoor]->wall = (yyvsp[(7) - (9)].i);
			    tmprdoor[ndoor]->pos = (yyvsp[(9) - (9)].i);
			    ndoor++;
			    if (ndoor >= MAX_OF_TYPE) {
				    yyerror("Too many doors in room!");
				    ndoor--;
			    }
			}
		  }
    break;

  case 76:
#line 622 "lev_comp.y"
    {
			maze.filling = (schar) (yyvsp[(5) - (5)].i);
			if (index((yyvsp[(3) - (5)].map), '.'))
			    yyerror("Invalid dot ('.') in level name.");
			if ((int) strlen((yyvsp[(3) - (5)].map)) > 8)
			    yyerror("Level names limited to 8 characters.");
			(yyval.map) = (yyvsp[(3) - (5)].map);
			in_room = 0;
			n_plist = n_mlist = n_olist = 0;
		  }
    break;

  case 77:
#line 635 "lev_comp.y"
    {
			(yyval.i) = get_floor_type((char)(yyvsp[(1) - (1)].i));
		  }
    break;

  case 78:
#line 639 "lev_comp.y"
    {
			(yyval.i) = -1;
		  }
    break;

  case 81:
#line 649 "lev_comp.y"
    {
			store_part();
		  }
    break;

  case 82:
#line 655 "lev_comp.y"
    {
			tmppart[npart] = New(mazepart);
			tmppart[npart]->halign = 1;
			tmppart[npart]->valign = 1;
			tmppart[npart]->nrobjects = 0;
			tmppart[npart]->nloc = 0;
			tmppart[npart]->nrmonst = 0;
			tmppart[npart]->xsize = 1;
			tmppart[npart]->ysize = 1;
			tmppart[npart]->map = (char **) alloc(sizeof(char *));
			tmppart[npart]->map[0] = (char *) alloc(1);
			tmppart[npart]->map[0][0] = STONE;
			max_x_map = COLNO-1;
			max_y_map = ROWNO;
		  }
    break;

  case 83:
#line 671 "lev_comp.y"
    {
			tmppart[npart] = New(mazepart);
			tmppart[npart]->halign = (yyvsp[(1) - (2)].i) % 10;
			tmppart[npart]->valign = (yyvsp[(1) - (2)].i) / 10;
			tmppart[npart]->nrobjects = 0;
			tmppart[npart]->nloc = 0;
			tmppart[npart]->nrmonst = 0;
			scan_map((yyvsp[(2) - (2)].map));
			Free((yyvsp[(2) - (2)].map));
		  }
    break;

  case 84:
#line 684 "lev_comp.y"
    {
			(yyval.i) = (yyvsp[(3) - (5)].i) + ((yyvsp[(5) - (5)].i) * 10);
		  }
    break;

  case 91:
#line 702 "lev_comp.y"
    {
			if (tmppart[npart]->nrobjects) {
			    yyerror("Object registers already initialized!");
			} else {
			    tmppart[npart]->robjects = (char *)alloc(n_olist);
			    (void) memcpy((genericptr_t)tmppart[npart]->robjects,
					  (genericptr_t)olist, n_olist);
			    tmppart[npart]->nrobjects = n_olist;
			}
		  }
    break;

  case 92:
#line 713 "lev_comp.y"
    {
			if (tmppart[npart]->nloc) {
			    yyerror("Location registers already initialized!");
			} else {
			    register int i;
			    tmppart[npart]->rloc_x = (char *) alloc(n_plist);
			    tmppart[npart]->rloc_y = (char *) alloc(n_plist);
			    for(i=0;i<n_plist;i++) {
				tmppart[npart]->rloc_x[i] = plist[i].x;
				tmppart[npart]->rloc_y[i] = plist[i].y;
			    }
			    tmppart[npart]->nloc = n_plist;
			}
		  }
    break;

  case 93:
#line 728 "lev_comp.y"
    {
			if (tmppart[npart]->nrmonst) {
			    yyerror("Monster registers already initialized!");
			} else {
			    tmppart[npart]->rmonst = (char *) alloc(n_mlist);
			    (void) memcpy((genericptr_t)tmppart[npart]->rmonst,
					  (genericptr_t)mlist, n_mlist);
			    tmppart[npart]->nrmonst = n_mlist;
			}
		  }
    break;

  case 94:
#line 741 "lev_comp.y"
    {
			if (n_olist < MAX_REGISTERS)
			    olist[n_olist++] = (yyvsp[(1) - (1)].i);
			else
			    yyerror("Object list too long!");
		  }
    break;

  case 95:
#line 748 "lev_comp.y"
    {
			if (n_olist < MAX_REGISTERS)
			    olist[n_olist++] = (yyvsp[(1) - (3)].i);
			else
			    yyerror("Object list too long!");
		  }
    break;

  case 96:
#line 757 "lev_comp.y"
    {
			if (n_mlist < MAX_REGISTERS)
			    mlist[n_mlist++] = (yyvsp[(1) - (1)].i);
			else
			    yyerror("Monster list too long!");
		  }
    break;

  case 97:
#line 764 "lev_comp.y"
    {
			if (n_mlist < MAX_REGISTERS)
			    mlist[n_mlist++] = (yyvsp[(1) - (3)].i);
			else
			    yyerror("Monster list too long!");
		  }
    break;

  case 98:
#line 773 "lev_comp.y"
    {
			if (n_plist < MAX_REGISTERS)
			    plist[n_plist++] = current_coord;
			else
			    yyerror("Location list too long!");
		  }
    break;

  case 99:
#line 780 "lev_comp.y"
    {
			if (n_plist < MAX_REGISTERS)
			    plist[n_plist++] = current_coord;
			else
			    yyerror("Location list too long!");
		  }
    break;

  case 123:
#line 816 "lev_comp.y"
    {
			tmpmonst[nmons] = New(monster);
			tmpmonst[nmons]->x = current_coord.x;
			tmpmonst[nmons]->y = current_coord.y;
			tmpmonst[nmons]->class = (yyvsp[(4) - (8)].i);
			tmpmonst[nmons]->peaceful = -1; /* no override */
			tmpmonst[nmons]->asleep = -1;
			tmpmonst[nmons]->align = - MAX_REGISTERS - 2;
			tmpmonst[nmons]->name.str = 0;
			tmpmonst[nmons]->appear = 0;
			tmpmonst[nmons]->appear_as.str = 0;
			tmpmonst[nmons]->chance = (yyvsp[(2) - (8)].i);
			tmpmonst[nmons]->id = NON_PM;
			if (!in_room)
			    check_coord(current_coord.x, current_coord.y,
					"Monster");
			if ((yyvsp[(6) - (8)].map)) {
			    int token = get_monster_id((yyvsp[(6) - (8)].map), (char) (yyvsp[(4) - (8)].i));
			    if (token == ERR)
				yywarning(
			      "Invalid monster name!  Making random monster.");
			    else
				tmpmonst[nmons]->id = token;
			    Free((yyvsp[(6) - (8)].map));
			}
		  }
    break;

  case 124:
#line 843 "lev_comp.y"
    {
			if (++nmons >= MAX_OF_TYPE) {
			    yyerror("Too many monsters in room or mazepart!");
			    nmons--;
			}
		  }
    break;

  case 127:
#line 856 "lev_comp.y"
    {
			tmpmonst[nmons]->name.str = (yyvsp[(2) - (2)].map);
		  }
    break;

  case 128:
#line 860 "lev_comp.y"
    {
			tmpmonst[nmons]->peaceful = (yyvsp[(2) - (2)].i);
		  }
    break;

  case 129:
#line 864 "lev_comp.y"
    {
			tmpmonst[nmons]->asleep = (yyvsp[(2) - (2)].i);
		  }
    break;

  case 130:
#line 868 "lev_comp.y"
    {
			tmpmonst[nmons]->align = (yyvsp[(2) - (2)].i);
		  }
    break;

  case 131:
#line 872 "lev_comp.y"
    {
			tmpmonst[nmons]->appear = (yyvsp[(2) - (3)].i);
			tmpmonst[nmons]->appear_as.str = (yyvsp[(3) - (3)].map);
		  }
    break;

  case 132:
#line 879 "lev_comp.y"
    {
		  }
    break;

  case 133:
#line 882 "lev_comp.y"
    {
			/* 1: is contents of preceeding object with 2 */
			/* 2: is a container */
			/* 0: neither */
			tmpobj[nobj-1]->containment = 2;
		  }
    break;

  case 134:
#line 891 "lev_comp.y"
    {
			tmpobj[nobj] = New(object);
			tmpobj[nobj]->class = (yyvsp[(3) - (5)].i);
			tmpobj[nobj]->corpsenm = NON_PM;
			tmpobj[nobj]->curse_state = -1;
			tmpobj[nobj]->name.str = 0;
			tmpobj[nobj]->chance = (yyvsp[(1) - (5)].i);
			tmpobj[nobj]->id = -1;
			if ((yyvsp[(5) - (5)].map)) {
			    int token = get_object_id((yyvsp[(5) - (5)].map), (yyvsp[(3) - (5)].i));
			    if (token == ERR)
				yywarning(
				"Illegal object name!  Making random object.");
			     else
				tmpobj[nobj]->id = token;
			    Free((yyvsp[(5) - (5)].map));
			}
		  }
    break;

  case 135:
#line 910 "lev_comp.y"
    {
			if (++nobj >= MAX_OF_TYPE) {
			    yyerror("Too many objects in room or mazepart!");
			    nobj--;
			}
		  }
    break;

  case 136:
#line 919 "lev_comp.y"
    {
			tmpobj[nobj]->containment = 0;
			tmpobj[nobj]->x = current_coord.x;
			tmpobj[nobj]->y = current_coord.y;
			if (!in_room)
			    check_coord(current_coord.x, current_coord.y,
					"Object");
		  }
    break;

  case 137:
#line 928 "lev_comp.y"
    {
			tmpobj[nobj]->containment = 1;
			/* random coordinate, will be overridden anyway */
			tmpobj[nobj]->x = -MAX_REGISTERS-1;
			tmpobj[nobj]->y = -MAX_REGISTERS-1;
		  }
    break;

  case 138:
#line 937 "lev_comp.y"
    {
			tmpobj[nobj]->spe = -127;
	/* Note below: we're trying to make as many of these optional as
	 * possible.  We clearly can't make curse_state, enchantment, and
	 * monster_id _all_ optional, since ",random" would be ambiguous.
	 * We can't even just make enchantment mandatory, since if we do that
	 * alone, ",random" requires too much lookahead to parse.
	 */
		  }
    break;

  case 139:
#line 947 "lev_comp.y"
    {
		  }
    break;

  case 140:
#line 950 "lev_comp.y"
    {
		  }
    break;

  case 141:
#line 953 "lev_comp.y"
    {
		  }
    break;

  case 142:
#line 958 "lev_comp.y"
    {
			tmpobj[nobj]->curse_state = -1;
		  }
    break;

  case 143:
#line 962 "lev_comp.y"
    {
			tmpobj[nobj]->curse_state = (yyvsp[(1) - (1)].i);
		  }
    break;

  case 144:
#line 968 "lev_comp.y"
    {
			int token = get_monster_id((yyvsp[(1) - (1)].map), (char)0);
			if (token == ERR)	/* "random" */
			    tmpobj[nobj]->corpsenm = NON_PM - 1;
			else
			    tmpobj[nobj]->corpsenm = token;
			Free((yyvsp[(1) - (1)].map));
		  }
    break;

  case 145:
#line 979 "lev_comp.y"
    {
			tmpobj[nobj]->spe = -127;
		  }
    break;

  case 146:
#line 983 "lev_comp.y"
    {
			tmpobj[nobj]->spe = (yyvsp[(1) - (1)].i);
		  }
    break;

  case 148:
#line 990 "lev_comp.y"
    {
		  }
    break;

  case 149:
#line 993 "lev_comp.y"
    {
			tmpobj[nobj]->name.str = (yyvsp[(2) - (2)].map);
		  }
    break;

  case 150:
#line 999 "lev_comp.y"
    {
			tmpdoor[ndoor] = New(door);
			tmpdoor[ndoor]->x = current_coord.x;
			tmpdoor[ndoor]->y = current_coord.y;
			tmpdoor[ndoor]->mask = (yyvsp[(3) - (5)].i);
			if(current_coord.x >= 0 && current_coord.y >= 0 &&
			   tmpmap[current_coord.y][current_coord.x] != DOOR &&
			   tmpmap[current_coord.y][current_coord.x] != SDOOR)
			    yyerror("Door decl doesn't match the map");
			ndoor++;
			if (ndoor >= MAX_OF_TYPE) {
				yyerror("Too many doors in mazepart!");
				ndoor--;
			}
		  }
    break;

  case 151:
#line 1017 "lev_comp.y"
    {
			tmptrap[ntrap] = New(trap);
			tmptrap[ntrap]->x = current_coord.x;
			tmptrap[ntrap]->y = current_coord.y;
			tmptrap[ntrap]->type = (yyvsp[(4) - (6)].i);
			tmptrap[ntrap]->chance = (yyvsp[(2) - (6)].i);
			if (!in_room)
			    check_coord(current_coord.x, current_coord.y,
					"Trap");
			if (++ntrap >= MAX_OF_TYPE) {
				yyerror("Too many traps in room or mazepart!");
				ntrap--;
			}
		  }
    break;

  case 152:
#line 1034 "lev_comp.y"
    {
		        int x, y, dir;

			tmpdb[ndb] = New(drawbridge);
			x = tmpdb[ndb]->x = current_coord.x;
			y = tmpdb[ndb]->y = current_coord.y;
			/* convert dir from a DIRECTION to a DB_DIR */
			dir = (yyvsp[(5) - (7)].i);
			switch(dir) {
			case W_NORTH: dir = DB_NORTH; y--; break;
			case W_SOUTH: dir = DB_SOUTH; y++; break;
			case W_EAST:  dir = DB_EAST;  x++; break;
			case W_WEST:  dir = DB_WEST;  x--; break;
			default:
			    yyerror("Invalid drawbridge direction");
			    break;
			}
			tmpdb[ndb]->dir = dir;
			if (current_coord.x >= 0 && current_coord.y >= 0 &&
			    !IS_WALL(tmpmap[y][x])) {
			    char ebuf[60];
			    Sprintf(ebuf,
				    "Wall needed for drawbridge (%02d, %02d)",
				    current_coord.x, current_coord.y);
			    yyerror(ebuf);
			}

			if ( (yyvsp[(7) - (7)].i) == D_ISOPEN )
			    tmpdb[ndb]->db_open = 1;
			else if ( (yyvsp[(7) - (7)].i) == D_CLOSED )
			    tmpdb[ndb]->db_open = 0;
			else
			    yyerror("A drawbridge can only be open or closed!");
			ndb++;
			if (ndb >= MAX_OF_TYPE) {
				yyerror("Too many drawbridges in mazepart!");
				ndb--;
			}
		   }
    break;

  case 153:
#line 1076 "lev_comp.y"
    {
			tmpwalk[nwalk] = New(walk);
			tmpwalk[nwalk]->x = current_coord.x;
			tmpwalk[nwalk]->y = current_coord.y;
			tmpwalk[nwalk]->dir = (yyvsp[(5) - (5)].i);
			nwalk++;
			if (nwalk >= MAX_OF_TYPE) {
				yyerror("Too many mazewalks in mazepart!");
				nwalk--;
			}
		  }
    break;

  case 154:
#line 1090 "lev_comp.y"
    {
			wallify_map();
		  }
    break;

  case 155:
#line 1096 "lev_comp.y"
    {
			tmplad[nlad] = New(lad);
			tmplad[nlad]->x = current_coord.x;
			tmplad[nlad]->y = current_coord.y;
			tmplad[nlad]->up = (yyvsp[(5) - (5)].i);
			if (!in_room)
			    check_coord(current_coord.x, current_coord.y,
					"Ladder");
			nlad++;
			if (nlad >= MAX_OF_TYPE) {
				yyerror("Too many ladders in mazepart!");
				nlad--;
			}
		  }
    break;

  case 156:
#line 1113 "lev_comp.y"
    {
			tmpstair[nstair] = New(stair);
			tmpstair[nstair]->x = current_coord.x;
			tmpstair[nstair]->y = current_coord.y;
			tmpstair[nstair]->up = (yyvsp[(5) - (5)].i);
			if (!in_room)
			    check_coord(current_coord.x, current_coord.y,
					"Stairway");
			nstair++;
			if (nstair >= MAX_OF_TYPE) {
				yyerror("Too many stairs in room or mazepart!");
				nstair--;
			}
		  }
    break;

  case 157:
#line 1130 "lev_comp.y"
    {
			tmplreg[nlreg] = New(lev_region);
			tmplreg[nlreg]->in_islev = (yyvsp[(3) - (3)].i);
			tmplreg[nlreg]->inarea.x1 = current_region.x1;
			tmplreg[nlreg]->inarea.y1 = current_region.y1;
			tmplreg[nlreg]->inarea.x2 = current_region.x2;
			tmplreg[nlreg]->inarea.y2 = current_region.y2;
		  }
    break;

  case 158:
#line 1139 "lev_comp.y"
    {
			tmplreg[nlreg]->del_islev = (yyvsp[(6) - (8)].i);
			tmplreg[nlreg]->delarea.x1 = current_region.x1;
			tmplreg[nlreg]->delarea.y1 = current_region.y1;
			tmplreg[nlreg]->delarea.x2 = current_region.x2;
			tmplreg[nlreg]->delarea.y2 = current_region.y2;
			if((yyvsp[(8) - (8)].i))
			    tmplreg[nlreg]->rtype = LR_UPSTAIR;
			else
			    tmplreg[nlreg]->rtype = LR_DOWNSTAIR;
			tmplreg[nlreg]->rname.str = 0;
			nlreg++;
			if (nlreg >= MAX_OF_TYPE) {
				yyerror("Too many levregions in mazepart!");
				nlreg--;
			}
		  }
    break;

  case 159:
#line 1159 "lev_comp.y"
    {
			tmplreg[nlreg] = New(lev_region);
			tmplreg[nlreg]->in_islev = (yyvsp[(3) - (3)].i);
			tmplreg[nlreg]->inarea.x1 = current_region.x1;
			tmplreg[nlreg]->inarea.y1 = current_region.y1;
			tmplreg[nlreg]->inarea.x2 = current_region.x2;
			tmplreg[nlreg]->inarea.y2 = current_region.y2;
		  }
    break;

  case 160:
#line 1168 "lev_comp.y"
    {
			tmplreg[nlreg]->del_islev = (yyvsp[(6) - (8)].i);
			tmplreg[nlreg]->delarea.x1 = current_region.x1;
			tmplreg[nlreg]->delarea.y1 = current_region.y1;
			tmplreg[nlreg]->delarea.x2 = current_region.x2;
			tmplreg[nlreg]->delarea.y2 = current_region.y2;
			tmplreg[nlreg]->rtype = LR_PORTAL;
			tmplreg[nlreg]->rname.str = (yyvsp[(8) - (8)].map);
			nlreg++;
			if (nlreg >= MAX_OF_TYPE) {
				yyerror("Too many levregions in mazepart!");
				nlreg--;
			}
		  }
    break;

  case 161:
#line 1185 "lev_comp.y"
    {
			tmplreg[nlreg] = New(lev_region);
			tmplreg[nlreg]->in_islev = (yyvsp[(3) - (3)].i);
			tmplreg[nlreg]->inarea.x1 = current_region.x1;
			tmplreg[nlreg]->inarea.y1 = current_region.y1;
			tmplreg[nlreg]->inarea.x2 = current_region.x2;
			tmplreg[nlreg]->inarea.y2 = current_region.y2;
		  }
    break;

  case 162:
#line 1194 "lev_comp.y"
    {
			tmplreg[nlreg]->del_islev = (yyvsp[(6) - (6)].i);
			tmplreg[nlreg]->delarea.x1 = current_region.x1;
			tmplreg[nlreg]->delarea.y1 = current_region.y1;
			tmplreg[nlreg]->delarea.x2 = current_region.x2;
			tmplreg[nlreg]->delarea.y2 = current_region.y2;
		  }
    break;

  case 163:
#line 1202 "lev_comp.y"
    {
			switch((yyvsp[(8) - (8)].i)) {
			case -1: tmplreg[nlreg]->rtype = LR_TELE; break;
			case 0: tmplreg[nlreg]->rtype = LR_DOWNTELE; break;
			case 1: tmplreg[nlreg]->rtype = LR_UPTELE; break;
			}
			tmplreg[nlreg]->rname.str = 0;
			nlreg++;
			if (nlreg >= MAX_OF_TYPE) {
				yyerror("Too many levregions in mazepart!");
				nlreg--;
			}
		  }
    break;

  case 164:
#line 1218 "lev_comp.y"
    {
			tmplreg[nlreg] = New(lev_region);
			tmplreg[nlreg]->in_islev = (yyvsp[(3) - (3)].i);
			tmplreg[nlreg]->inarea.x1 = current_region.x1;
			tmplreg[nlreg]->inarea.y1 = current_region.y1;
			tmplreg[nlreg]->inarea.x2 = current_region.x2;
			tmplreg[nlreg]->inarea.y2 = current_region.y2;
		  }
    break;

  case 165:
#line 1227 "lev_comp.y"
    {
			tmplreg[nlreg]->del_islev = (yyvsp[(6) - (6)].i);
			tmplreg[nlreg]->delarea.x1 = current_region.x1;
			tmplreg[nlreg]->delarea.y1 = current_region.y1;
			tmplreg[nlreg]->delarea.x2 = current_region.x2;
			tmplreg[nlreg]->delarea.y2 = current_region.y2;
			tmplreg[nlreg]->rtype = LR_BRANCH;
			tmplreg[nlreg]->rname.str = 0;
			nlreg++;
			if (nlreg >= MAX_OF_TYPE) {
				yyerror("Too many levregions in mazepart!");
				nlreg--;
			}
		  }
    break;

  case 166:
#line 1244 "lev_comp.y"
    {
			(yyval.i) = -1;
		  }
    break;

  case 167:
#line 1248 "lev_comp.y"
    {
			(yyval.i) = (yyvsp[(2) - (2)].i);
		  }
    break;

  case 168:
#line 1254 "lev_comp.y"
    {
			(yyval.i) = 0;
		  }
    break;

  case 169:
#line 1258 "lev_comp.y"
    {
/* This series of if statements is a hack for MSC 5.1.  It seems that its
   tiny little brain cannot compile if these are all one big if statement. */
			if ((yyvsp[(3) - (10)].i) <= 0 || (yyvsp[(3) - (10)].i) >= COLNO)
				yyerror("Region out of level range!");
			else if ((yyvsp[(5) - (10)].i) < 0 || (yyvsp[(5) - (10)].i) >= ROWNO)
				yyerror("Region out of level range!");
			else if ((yyvsp[(7) - (10)].i) <= 0 || (yyvsp[(7) - (10)].i) >= COLNO)
				yyerror("Region out of level range!");
			else if ((yyvsp[(9) - (10)].i) < 0 || (yyvsp[(9) - (10)].i) >= ROWNO)
				yyerror("Region out of level range!");
			current_region.x1 = (yyvsp[(3) - (10)].i);
			current_region.y1 = (yyvsp[(5) - (10)].i);
			current_region.x2 = (yyvsp[(7) - (10)].i);
			current_region.y2 = (yyvsp[(9) - (10)].i);
			(yyval.i) = 1;
		  }
    break;

  case 170:
#line 1278 "lev_comp.y"
    {
			tmpfountain[nfountain] = New(fountain);
			tmpfountain[nfountain]->x = current_coord.x;
			tmpfountain[nfountain]->y = current_coord.y;
			if (!in_room)
			    check_coord(current_coord.x, current_coord.y,
					"Fountain");
			nfountain++;
			if (nfountain >= MAX_OF_TYPE) {
			    yyerror("Too many fountains in room or mazepart!");
			    nfountain--;
			}
		  }
    break;

  case 171:
#line 1294 "lev_comp.y"
    {
			tmpsink[nsink] = New(sink);
			tmpsink[nsink]->x = current_coord.x;
			tmpsink[nsink]->y = current_coord.y;
			nsink++;
			if (nsink >= MAX_OF_TYPE) {
				yyerror("Too many sinks in room!");
				nsink--;
			}
		  }
    break;

  case 172:
#line 1307 "lev_comp.y"
    {
			tmppool[npool] = New(pool);
			tmppool[npool]->x = current_coord.x;
			tmppool[npool]->y = current_coord.y;
			npool++;
			if (npool >= MAX_OF_TYPE) {
				yyerror("Too many pools in room!");
				npool--;
			}
		  }
    break;

  case 173:
#line 1320 "lev_comp.y"
    {
			tmpdig[ndig] = New(digpos);
			tmpdig[ndig]->x1 = current_region.x1;
			tmpdig[ndig]->y1 = current_region.y1;
			tmpdig[ndig]->x2 = current_region.x2;
			tmpdig[ndig]->y2 = current_region.y2;
			ndig++;
			if (ndig >= MAX_OF_TYPE) {
				yyerror("Too many diggables in mazepart!");
				ndig--;
			}
		  }
    break;

  case 174:
#line 1335 "lev_comp.y"
    {
			tmppass[npass] = New(digpos);
			tmppass[npass]->x1 = current_region.x1;
			tmppass[npass]->y1 = current_region.y1;
			tmppass[npass]->x2 = current_region.x2;
			tmppass[npass]->y2 = current_region.y2;
			npass++;
			if (npass >= 32) {
				yyerror("Too many passwalls in mazepart!");
				npass--;
			}
		  }
    break;

  case 175:
#line 1350 "lev_comp.y"
    {
			tmpreg[nreg] = New(region);
			tmpreg[nreg]->x1 = current_region.x1;
			tmpreg[nreg]->y1 = current_region.y1;
			tmpreg[nreg]->x2 = current_region.x2;
			tmpreg[nreg]->y2 = current_region.y2;
			tmpreg[nreg]->rlit = (yyvsp[(5) - (8)].i);
			tmpreg[nreg]->rtype = (yyvsp[(7) - (8)].i);
			if((yyvsp[(8) - (8)].i) & 1) tmpreg[nreg]->rtype += MAXRTYPE+1;
			tmpreg[nreg]->rirreg = (((yyvsp[(8) - (8)].i) & 2) != 0);
			if(current_region.x1 > current_region.x2 ||
			   current_region.y1 > current_region.y2)
			   yyerror("Region start > end!");
			if(tmpreg[nreg]->rtype == VAULT &&
			   (tmpreg[nreg]->rirreg ||
			    (tmpreg[nreg]->x2 - tmpreg[nreg]->x1 != 1) ||
			    (tmpreg[nreg]->y2 - tmpreg[nreg]->y1 != 1)))
				yyerror("Vaults must be exactly 2x2!");
			if(want_warnings && !tmpreg[nreg]->rirreg &&
			   current_region.x1 > 0 && current_region.y1 > 0 &&
			   current_region.x2 < (int)max_x_map &&
			   current_region.y2 < (int)max_y_map) {
			    /* check for walls in the room */
			    char ebuf[60];
			    register int x, y, nrock = 0;

			    for(y=current_region.y1; y<=current_region.y2; y++)
				for(x=current_region.x1;
				    x<=current_region.x2; x++)
				    if(IS_ROCK(tmpmap[y][x]) ||
				       IS_DOOR(tmpmap[y][x])) nrock++;
			    if(nrock) {
				Sprintf(ebuf,
					"Rock in room (%02d,%02d,%02d,%02d)?!",
					current_region.x1, current_region.y1,
					current_region.x2, current_region.y2);
				yywarning(ebuf);
			    }
			    if (
		!IS_ROCK(tmpmap[current_region.y1-1][current_region.x1-1]) ||
		!IS_ROCK(tmpmap[current_region.y2+1][current_region.x1-1]) ||
		!IS_ROCK(tmpmap[current_region.y1-1][current_region.x2+1]) ||
		!IS_ROCK(tmpmap[current_region.y2+1][current_region.x2+1])) {
				Sprintf(ebuf,
				"NonRock edge in room (%02d,%02d,%02d,%02d)?!",
					current_region.x1, current_region.y1,
					current_region.x2, current_region.y2);
				yywarning(ebuf);
			    }
			} else if(tmpreg[nreg]->rirreg &&
		!IS_ROOM(tmpmap[current_region.y1][current_region.x1])) {
			    char ebuf[60];
			    Sprintf(ebuf,
				    "Rock in irregular room (%02d,%02d)?!",
				    current_region.x1, current_region.y1);
			    yyerror(ebuf);
			}
			nreg++;
			if (nreg >= MAX_OF_TYPE) {
				yyerror("Too many regions in mazepart!");
				nreg--;
			}
		  }
    break;

  case 176:
#line 1416 "lev_comp.y"
    {
			tmpaltar[naltar] = New(altar);
			tmpaltar[naltar]->x = current_coord.x;
			tmpaltar[naltar]->y = current_coord.y;
			tmpaltar[naltar]->align = (yyvsp[(5) - (7)].i);
			tmpaltar[naltar]->shrine = (yyvsp[(7) - (7)].i);
			if (!in_room)
			    check_coord(current_coord.x, current_coord.y,
					"Altar");
			naltar++;
			if (naltar >= MAX_OF_TYPE) {
				yyerror("Too many altars in room or mazepart!");
				naltar--;
			}
		  }
    break;

  case 177:
#line 1434 "lev_comp.y"
    {
			tmpgold[ngold] = New(gold);
			tmpgold[ngold]->x = current_coord.x;
			tmpgold[ngold]->y = current_coord.y;
			tmpgold[ngold]->amount = (yyvsp[(3) - (5)].i);
			if (!in_room)
			    check_coord(current_coord.x, current_coord.y,
					"Gold");
			ngold++;
			if (ngold >= MAX_OF_TYPE) {
				yyerror("Too many golds in room or mazepart!");
				ngold--;
			}
		  }
    break;

  case 178:
#line 1451 "lev_comp.y"
    {
			tmpengraving[nengraving] = New(engraving);
			tmpengraving[nengraving]->x = current_coord.x;
			tmpengraving[nengraving]->y = current_coord.y;
			tmpengraving[nengraving]->engr.str = (yyvsp[(7) - (7)].map);
			tmpengraving[nengraving]->etype = (yyvsp[(5) - (7)].i);
			if (!in_room)
			    check_coord(current_coord.x, current_coord.y,
					"Engraving");
			nengraving++;
			if (nengraving >= MAX_OF_TYPE) {
			    yyerror("Too many engravings in room or mazepart!");
			    nengraving--;
			}
		  }
    break;

  case 180:
#line 1470 "lev_comp.y"
    {
			(yyval.i) = - MAX_REGISTERS - 1;
		  }
    break;

  case 183:
#line 1478 "lev_comp.y"
    {
			(yyval.i) = - MAX_REGISTERS - 1;
		  }
    break;

  case 186:
#line 1486 "lev_comp.y"
    {
			(yyval.map) = (char *) 0;
		  }
    break;

  case 188:
#line 1493 "lev_comp.y"
    {
			(yyval.map) = (char *) 0;
		  }
    break;

  case 189:
#line 1499 "lev_comp.y"
    {
			int token = get_trap_type((yyvsp[(1) - (1)].map));
			if (token == ERR)
				yyerror("Unknown trap type!");
			(yyval.i) = token;
			Free((yyvsp[(1) - (1)].map));
		  }
    break;

  case 191:
#line 1510 "lev_comp.y"
    {
			int token = get_room_type((yyvsp[(1) - (1)].map));
			if (token == ERR) {
				yywarning("Unknown room type!  Making ordinary room...");
				(yyval.i) = OROOM;
			} else
				(yyval.i) = token;
			Free((yyvsp[(1) - (1)].map));
		  }
    break;

  case 193:
#line 1523 "lev_comp.y"
    {
			(yyval.i) = 0;
		  }
    break;

  case 194:
#line 1527 "lev_comp.y"
    {
			(yyval.i) = (yyvsp[(2) - (2)].i);
		  }
    break;

  case 195:
#line 1531 "lev_comp.y"
    {
			(yyval.i) = (yyvsp[(2) - (4)].i) + ((yyvsp[(4) - (4)].i) << 1);
		  }
    break;

  case 198:
#line 1539 "lev_comp.y"
    {
			current_coord.x = current_coord.y = -MAX_REGISTERS-1;
		  }
    break;

  case 205:
#line 1555 "lev_comp.y"
    {
			(yyval.i) = - MAX_REGISTERS - 1;
		  }
    break;

  case 208:
#line 1565 "lev_comp.y"
    {
			if ( (yyvsp[(3) - (4)].i) >= MAX_REGISTERS )
				yyerror("Register Index overflow!");
			else
				current_coord.x = current_coord.y = - (yyvsp[(3) - (4)].i) - 1;
		  }
    break;

  case 209:
#line 1574 "lev_comp.y"
    {
			if ( (yyvsp[(3) - (4)].i) >= MAX_REGISTERS )
				yyerror("Register Index overflow!");
			else
				(yyval.i) = - (yyvsp[(3) - (4)].i) - 1;
		  }
    break;

  case 210:
#line 1583 "lev_comp.y"
    {
			if ( (yyvsp[(3) - (4)].i) >= MAX_REGISTERS )
				yyerror("Register Index overflow!");
			else
				(yyval.i) = - (yyvsp[(3) - (4)].i) - 1;
		  }
    break;

  case 211:
#line 1592 "lev_comp.y"
    {
			if ( (yyvsp[(3) - (4)].i) >= 3 )
				yyerror("Register Index overflow!");
			else
				(yyval.i) = - (yyvsp[(3) - (4)].i) - 1;
		  }
    break;

  case 213:
#line 1604 "lev_comp.y"
    {
			if (check_monster_char((char) (yyvsp[(1) - (1)].i)))
				(yyval.i) = (yyvsp[(1) - (1)].i) ;
			else {
				yyerror("Unknown monster class!");
				(yyval.i) = ERR;
			}
		  }
    break;

  case 214:
#line 1615 "lev_comp.y"
    {
			char c = (yyvsp[(1) - (1)].i);
			if (check_object_char(c))
				(yyval.i) = c;
			else {
				yyerror("Unknown char class!");
				(yyval.i) = ERR;
			}
		  }
    break;

  case 218:
#line 1634 "lev_comp.y"
    {
			(yyval.i) = 100;	/* default is 100% */
		  }
    break;

  case 219:
#line 1638 "lev_comp.y"
    {
			if ((yyvsp[(1) - (1)].i) <= 0 || (yyvsp[(1) - (1)].i) > 100)
			    yyerror("Expected percentile chance.");
			(yyval.i) = (yyvsp[(1) - (1)].i);
		  }
    break;

  case 222:
#line 1650 "lev_comp.y"
    {
			if (!in_room && !init_lev.init_present &&
			    ((yyvsp[(2) - (5)].i) < 0 || (yyvsp[(2) - (5)].i) > (int)max_x_map ||
			     (yyvsp[(4) - (5)].i) < 0 || (yyvsp[(4) - (5)].i) > (int)max_y_map))
			    yyerror("Coordinates out of map range!");
			current_coord.x = (yyvsp[(2) - (5)].i);
			current_coord.y = (yyvsp[(4) - (5)].i);
		  }
    break;

  case 223:
#line 1661 "lev_comp.y"
    {
/* This series of if statements is a hack for MSC 5.1.  It seems that its
   tiny little brain cannot compile if these are all one big if statement. */
			if ((yyvsp[(2) - (9)].i) < 0 || (yyvsp[(2) - (9)].i) > (int)max_x_map)
				yyerror("Region out of map range!");
			else if ((yyvsp[(4) - (9)].i) < 0 || (yyvsp[(4) - (9)].i) > (int)max_y_map)
				yyerror("Region out of map range!");
			else if ((yyvsp[(6) - (9)].i) < 0 || (yyvsp[(6) - (9)].i) > (int)max_x_map)
				yyerror("Region out of map range!");
			else if ((yyvsp[(8) - (9)].i) < 0 || (yyvsp[(8) - (9)].i) > (int)max_y_map)
				yyerror("Region out of map range!");
			current_region.x1 = (yyvsp[(2) - (9)].i);
			current_region.y1 = (yyvsp[(4) - (9)].i);
			current_region.x2 = (yyvsp[(6) - (9)].i);
			current_region.y2 = (yyvsp[(8) - (9)].i);
		  }
    break;


/* Line 1267 of yacc.c.  */
#line 3594 "y.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 1679 "lev_comp.y"


/*lev_comp.y*/

