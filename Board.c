#include "Board.h"

static void 
setup_num(struct board *me)
{
	int x, y, j, i, 
		w = me->width, 
		h = me->height,
		count = 0;

	struct tile *grid_tile = &me->tiles[0]; 

	while(count < me->width * me->height)
	{
		if(grid_tile->is_bomb)
		{
			for(i = -1; i <= 1; ++i)
			{
				for(j = -1; j <= 1; ++j)
				{
					int x = grid_tile->x,
						y = grid_tile->y;

					if(!me->tiles[(y+i) * w + x+j].is_bomb && y+i < me->height && y+i >= 0 && x+j < me->width && x+j >= 0)
					{
						me->tiles[(y+i) * w + x+j].adj_bombs += 1;
					}
				}
			}
		}
		++grid_tile;	
		++count;
	}
}


static void 
initialize(struct board *me)
{
	int count = 0;
	struct tile *grid_tile = &me->tiles[0]; 

	while(count < me->width * me->height)
	{
		tile_ctor(grid_tile, count % me->width, count / me->width, count < me->b_count);
		++grid_tile;	
		++count;	
	}
}


static int 
reset_tiles(struct board *me, int new_x, int new_y)
{
	struct tile *temp = NULL;

	if(new_y != me->height || new_x != me->width)
	{
		temp = realloc(me->tiles, new_y * new_x * sizeof(struct tile));
	}
	else
	{
		temp = me->tiles;
	}

	if(temp != NULL)
	{
		if(me->tiles != temp)
		{
			me->tiles = temp;
		}
		return 1;
	}
	return 0; // catch the 0 and end the game
}

int
reset(struct board *me, int new_x, int new_y, int new_bombs)
{
	if(me->width != new_x || me->height != new_y || new_bombs != me->b_count)
	{
		if(!reset_tiles(me, new_x, new_y))
		{
			return 0; // failed to reallocate 
		}
		me->b_count = new_bombs;
		me->width = new_x;
		me->height = new_y;	
	}

	initialize(me);

	int count = 0;
	struct tile *grid_tile = &me->tiles[0]; 

	while(count < me->width * me->height)
	{
		int ri = grid_tile->y + (random() % (me->height - grid_tile->y));
		int rj = grid_tile->x + (random() % (me->width - grid_tile->x));

		int temp = grid_tile->is_bomb;
		grid_tile->is_bomb = me->tiles[ri * me->width + rj].is_bomb;
		me->tiles[ri * me->width + rj].is_bomb = temp;

		++grid_tile;	
		++count;
	}
	setup_num(me);
	return 1;
}

void
board_ctor(struct board *me, int x, int y, int num)
{
	me->tiles = malloc(y * x * sizeof(struct tile));
	me->width = x; 
	me->height = y;
	me->b_count = num; 
	initialize(me);
}
