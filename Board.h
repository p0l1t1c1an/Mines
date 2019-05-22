#ifndef BOARD_H
#define BOARD_H

#include "Tile.h"

typedef struct board{
	tile ***tiles;
	int bomb_amount;
	bool end;
	struct board_VTable *p_Table;
}board;

typedef struct board_VTable{
	void (*p_reveal)	(board *me, int x, int y, GtkWidget *g);
	void (*p_new_game)	(board *me, int x, int y, GtkWidget *g);
	void (*p_setup_num)	(board *me, int x, int y); // sets the number for each tile in board
}board_VTable;

board_VTable *board_table_ctor();
board *board_ctor(board *me, int x, int y, int num_bombs);


#endif//BOARD_H
