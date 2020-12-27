/* astar.c	*/
/* NetHack Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "astar.h"

//TODO:
/// Generalize for use with any pathfinding (allow the use of MM_ flags
/// Malloc path and use the path pointer to return it

boolean
a_star(x, y, gx, gy, mmflags, max_length, path)
int x;
int y;
int gx;
int gy;
long mmflags;
int max_length;
struct pathNode **path;
{
	struct mapNode closed[COLNO][ROWNO];
	(void) memset((genericptr_t) closed, 0, sizeof(closed));
	int hx, hy;
	int i, j;
	int nxtx, nxty;
	int g, f;
	boolean succeeded = FALSE;
	boolean propagate = FALSE;
	struct frontNode *queue = 0;
	struct frontNode *curnode;
	struct frontNode *nextnode;
	struct frontNode *insert;
	//Set up first node
	curnode = (struct frontNode *) malloc(sizeof(struct frontNode));
	curnode->x = x;
	curnode->y = y;
	//Origin node == current node means this was the start of the path.
	curnode->from_x = x;
	curnode->from_y = y;
	curnode->g = 0;
	hx = h(x, gx);
	hy = h(y, gy);
	curnode->f = max(hx, hy);
	curnode->propagate = TRUE;
	curnode->next = queue;
	queue = curnode;
	//Keep going as long as there are nodes left in the queue
	while(queue){
		//pop queue
		curnode = queue;
		queue = queue->next;
		//update closed set
		closed[curnode->x][curnode->y].g = curnode->g;
		closed[curnode->x][curnode->y].from_x = curnode->from_x;
		closed[curnode->x][curnode->y].from_y = curnode->from_y;
		
		if(curnode->x == gx && curnode->y == gy){
			succeeded = TRUE;
			free(curnode);
			break;
		}
		
		
		//possibly add neighbors to open queue
		g = curnode->g+1;
		if(curnode->propagate){
		 for(i = -1; i < 2; i++){
		  for(j = -1; j < 2; j++){
			//Don't re-add the same square
			if(!(i || j))
				continue;
			//Compute next x and y
			nxtx = curnode->x + i;
			nxty = curnode->y + j;
			propagate = ZAP_POS(levl[nxtx][nxty].typ);
			//If next location is out-of-bounds, continue.
			if(!isok(nxtx, nxty)){
				// pline("reject (%d,%d): out of bounds", nxtx, nxty);
				continue;
			}
			//Is this just a longer path to a previous node? if so continue.
			///Back at start
			if(nxtx == x && nxty == y){
				// pline("reject (%d,%d): origin", nxtx, nxty);
				continue;
			}
			///Back at previous node with no better path
			if(closed[nxtx][nxty].g && closed[nxtx][nxty].g <= g){
				// pline("reject (%d,%d): no better path %d vs %d", nxtx, nxty, closed[nxtx][nxty].g,  g);
				continue;
			}
			//Calculate f
			hx = h(nxtx, gx);
			hy = h(nxty, gy);
			f = g + max(hx, hy);
			//has f exceeded the limit? (The heuristic gives the minimum number of steps required to reach the goal from here)
			if(max_length && f > max_length){
				// pline("reject (%d,%d): estimate %d exceeds max length %d", nxtx, nxty, f,  max_length);
				continue;
			}
#define CREATE_NEXTNODE 	nextnode = (struct frontNode *) malloc(sizeof(struct frontNode));\
nextnode->x = nxtx;\
nextnode->y = nxty;\
nextnode->from_x = curnode->x;\
nextnode->from_y = curnode->y;\
nextnode->g = g;\
nextnode->f = f;\
nextnode->propagate = propagate;
// nextnode->propagate = TRUE;
			//Check if nxtx, nxty need to be added
			///Does it get added to the head? Yes if the queue is empty or the priority is low
			if(!queue || queue->f > f){
				CREATE_NEXTNODE
				nextnode->next = queue;
				queue = nextnode;
				// pline("(%d,%d): added to head of queue", nxtx, nxty);
				//insert = queue: begin checking for old (x,y) nodes at (one after) the new head
				insert = queue;
			///Is the head a better (nxtx, nxty)?
			} else if(queue->x == nxtx && queue->y == nxty){
				//old f <= new f, so no need to add
				// pline("reject (%d,%d): no worse on queue", nxtx, nxty);
				continue;
			///Does (nxtx, nxty) need to be handled after the head?
			} else {
#define nextslot (insert->next)
				//broken by break when nextslot is empty, an insert point is found, or better (x,y) is found
				for(insert = queue; insert; insert = insert->next){
					///Have we reached the end of the chain without finding (nxtx, nxty), or do we have a better f?
					if(!nextslot || nextslot->f > f){
						CREATE_NEXTNODE
						nextnode->next = nextslot;
						nextslot = nextnode;
						// pline("(%d,%d): added within queue", nxtx, nxty);
						//Begin checking one after the new node
						insert = nextnode;
						break;
					}
					///Have we found a better (nxtx, nxty) already on the chain?
					else if(nextslot->x == nxtx && nextslot->y == nxty){
						//No need to check for removals
						// pline("reject (%d,%d): no worse inside queue", nxtx, nxty);
						insert = (struct frontNode *) 0;
						break;
					}
					///No else, we will either find it, find where it goes, or reach the end of the chain without finding either.
				}
			}
			//If there's anything left, check it for an old (and worse) instance of (nxtx, nxty)
			if(insert) while(nextslot){
				if(nextslot->x == nxtx && nextslot->y == nxty){
					nextnode = nextslot;
					nextslot = nextslot->next;
					free(nextnode);
					break;
				}
				insert = insert->next;
			}
#undef nextslot
		  }
		 }
		}
		//This node has been processed
		free(curnode);
	}
	//Clean up
	///Free remaining queue
	while(queue){
		curnode = queue;
		queue = queue->next;
		closed[curnode->x][curnode->y].g = curnode->g;
		free(curnode);
	}
	///If the caller wants it, malloc some memory to hold the path that was found
	if(succeeded && path){
		*path = (struct pathNode *) 0;
		impossible("Returning path is not implemented!");
	}
	/*
	//Dumps pathfinding data into a .csv in a format where Excel can color-code it
	if(TRUE){
		FILE *rfile;
		char pbuf[BUFSZ];
		rfile = fopen_datafile("Sightlines.csv", "a", SCOREPREFIX);
		pbuf[0] = 0;
		Sprintf(pbuf, "%d\n", (int) succeeded);
		fprintf(rfile, pbuf);
		for(j = 0; j < ROWNO; j++){
			pbuf[0] = 0;
			for(i = 0; i < COLNO; i++){
				Sprintf(eos(pbuf), "%d,", closed[i][j].g);
			}
			Sprintf(eos(pbuf), "\n");
			fprintf(rfile, pbuf);
		}
		Sprintf(pbuf, "\n");
		fprintf(rfile, pbuf);
		fclose(rfile);
	}
*/
	///Return value indicates if a path was found, and is always given
	return succeeded;
}


boolean
path_exists(x, y, gx, gy, mvflags, max_length)
int x;
int y;
int gx;
int gy;
long mvflags;
int max_length;
{
	return a_star(x, y, gx, gy, mvflags, max_length, (struct pathNode **) 0);
}

