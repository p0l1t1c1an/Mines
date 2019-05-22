#ifndef TILE_H
#define TILE_H

#include <stdbool.h>
#include <gtk/gtk.h>

typedef struct tile{
	bool is_bomb;
	bool is_flag;
	bool is_selected;
	int num_bombs;
	int x_pos;
	int y_pos;
	struct tile_VTable *p_Table;
}tile;

typedef struct tile_VTable {
	void (*p_selected)	(tile *me, GtkWidget *w);
	void (*p_set_flag)	(tile *me, GtkWidget *w);
	void (*p_set_bomb)	(tile *me);
}tile_VTable;

tile_VTable *tile_table_ctor();
void tile_ctor(tile *me, int x, int y);

#endif //TILE_H
