#include "Tile.h"

void 
toggle_flag(struct tile *me)
{
	if(!me->is_selected)
	{
		me->is_flag = !me->is_flag;
	}
}

void
tile_ctor(struct tile *me, int x, int y, int bomb)
{
	me->is_bomb = bomb; 
	me->is_flag = 0; 
	me->is_selected = 0; 
	me->adj_bombs = 0; 
	me->adj_flags = 0;
	me->x = x; 
	me->y = y;
}
