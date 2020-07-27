#include "Board.h"
#include <string.h>


static void 
add_bomb(struct board *me, struct tile *t)
{
	int j, i, x, y, 
		w = me->width, 
		h = me->height;
		 
	for(i = -1; i <= 1; ++i)
	{
		for(j = -1; j <= 1; ++j)
		{
			x = t->x;
			y = t->y;

			if(y+i < h && y+i >= 0 && x+j < w && x+j >= 0)
			{
				++(me->tiles[(y+i) * w + x+j].adj_bombs);
			}
		}
	}
}


static void 
initialize(struct board *me)
{
	int	w = me->width, 
		h = me->height,
		remaining = me->b_count,
		count;

	struct tile *grid_tile = &me->tiles[0]; 

	for(count = 0; count < w * h; ++count)
	{
		tile_ctor(grid_tile, count % w, count / w, random() % (w*h - count) < remaining);
		if(grid_tile->is_bomb)
		{
			add_bomb(me, grid_tile);	
			--remaining;
		}
		++grid_tile;	
	}
	printf("Remaining: %d\n", remaining);
	
	// catches if there are remaining bombs to be placed, 
	// which should be unlikely. But I haven't really
	// looked into the possibility of it happening. Or if it 
	// is possible to have a remainder at all.
	while(remaining > 0) 
	{
		grid_tile = &me->tiles[(random() % h) * w + (random() % w)];
		if(!grid_tile->is_bomb)
		{
			add_bomb(me, grid_tile);
			++(grid_tile->is_bomb);
			--remaining;
		}
	}
}

static int 
reset_tiles(struct board *me, size_t s)
{
	struct tile *temp = realloc(me->tiles, s);

	if(temp)
	{
		me->tiles = temp;
		return 1;
	}
	return 0; // catch the 0 and end the game
}

int
reset(struct board *me, int new_x, int new_y, int new_bombs)
{
	size_t s = new_y * new_x * sizeof *me->tiles;
	if(me->width != new_x || me->height != new_y) 
	{
		if(!reset_tiles(me, s))
		{
			return 0; // failed to reallocate 
		}
		me->width = new_x;
		me->height = new_y;	
	}	
	
	if(new_bombs != me->b_count)
	{
		me->b_count = new_bombs;
	}

	memset(me->tiles, 0, s);
	initialize(me);
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
