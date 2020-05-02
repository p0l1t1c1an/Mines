#include "Board.h"

static void 
setup_num(struct board *me)
{
	int i, j, k, l; 
	int w = me->width, h = me->height;
	for(i = 0; i < h; i++)
	{
		for(j = 0; j < w; j++)
		{
			if(me->tiles[i * w + j].is_bomb)
			{
				for(k = i-1; k <= i+1; k++)
				{
					for(l = j-1; l <= j+1; l++)
					{
						if(!me->tiles[k * w + l].is_bomb && k < h && k >= 0 && l < w && l >= 0)
						{
							me->tiles[k * w + l].adj_bombs += 1;
						}
					}
				}
			}
		}
	}
}


static void 
initialize(struct board *me)
{
	int i, j, count = 0;
	for(i = 0; i < me->height; ++i)
	{
		for (j = 0; j < me->width; ++j)
		{
			tile_ctor(&me->tiles[i* me->width + j], j, i, count < me->b_count);
			++count;	
		}
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

	int i, j;
	for(i = 0; i < me->height; i++)
	{
		for(j = 0; j < me->width; j++)
		{
			int ri = i + (random() % (me->height - i));
			int rj = j + (random() % (me->width - j));

			int temp = me->tiles[i * me->width + j].is_bomb;
			me->tiles[i * me->width + j].is_bomb = me->tiles[ri * me->width + rj].is_bomb;
			me->tiles[ri * me->width + rj].is_bomb = temp;
		}
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
