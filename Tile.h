#ifndef TILE_H
#define TILE_H

#include <stdio.h>

/*
 * Tile
 * Contains unsigned chars to store states of the tile.
 * These being if the tile is a bomb, flagged, selected,
 * positon, and the number of bombs surronding it.
 */

struct tile
{
	unsigned char 
		is_bomb, is_flag, is_selected,
		adj_bombs, adj_flags, x, y;
};

// toggles the state of the is_flag
void 
toggle_flag(struct tile *me);

// constructs a tile to a default state
void
tile_ctor(struct tile *me, int x, int y, int bomb);

#endif //TILE_H
