/* astar.h	*/
/* NetHack Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef ASTAR_H
#define ASTAR_H

struct mapNode {
	int from_x;
	int from_y;
	int g;
};

struct frontNode {
	int x;
	int y;
	int from_x;
	int from_y;
	boolean propagate;
	int g;
	int f;
	struct frontNode *next;
};

struct pathNode {
	int x;
	int y;
	int to_x;
	int to_y;
	struct frontNode *next;
};

//Don't do a function call for the heuristic
#define h(x, gl_x)	((x > gl_x) ? (x - gl_x) : (gl_x - x))




#endif