#ifndef BOARD_H
#define BOARD_H

#include "Tile.h"

#include <stdlib.h>
#include <time.h>

/* 
 * Board 
 * Contains an array of tiles and dtails about
 * the size, bomb count, and if board can be edited (end) 
 */
struct board
{
	unsigned char width, height, b_count;
	struct tile *tiles;
};

// resizes the board and resets the tiles and bombs 
int
reset(struct board *me, int new_x, int new_y, int new_b);
 
// allocates tiles and constructs the board to a default state
void
board_ctor(struct board *me, int x, int y, int num_bombs);

#endif //BOARD_H
