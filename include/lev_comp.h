/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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
/* Line 1489 of yacc.c.  */
#line 199 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

