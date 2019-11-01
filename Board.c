#include "Board.h"
#include <time.h>

extern char *set_directory(void);
#define directory set_directory()

//#define directory getcwd(NULL, 0)
#define images "/Images/"
#define bomb "Mine.svg"
#define wrong_flag "Wrong.svg"
#define face "Face.svg" // would be confusing to call it tile


static void ctor_each_tile(board *me, int x, int y){
	for(int i = 0; i < y; i++){
		for(int j = 0; j < x; j++){
			tile_ctor(me->tiles[i][j], j, i);
		}
	}
}

static void reveal(board *me, int x, int y, GtkWidget *grid){
	me->end = true;
	for(int i = 0; i < y; i++){
    	for(int j = 0; j < x; j++){
			if(me->tiles[i][j]->is_bomb || me->tiles[i][j]->is_flag){
				
				int len = (strlen(directory) + strlen(images) + strlen(bomb) + 1 );

				GtkWidget *ebox = gtk_grid_get_child_at(GTK_GRID(grid), j, i);
				GtkWidget *img  = gtk_bin_get_child(GTK_BIN(ebox));

				if(!me->tiles[i][j]->is_flag && me->tiles[i][j]->is_bomb){
					char dir[len];
					strcpy(dir, directory);
					strcat(dir, images);
					strcat(dir, bomb);

					gtk_image_set_from_file(GTK_IMAGE(img), dir);
				}

				else if(me->tiles[i][j]->is_flag && !me->tiles[i][j]->is_bomb){
					char dir[len];
					strcpy(dir, directory);
					strcat(dir, images);
					strcat(dir, wrong_flag);

					gtk_image_set_from_file(GTK_IMAGE(img), dir);
				}
			}
		}
	}
}

static void setup_num(board *me, int x, int y){
	for(int i = 0; i < y; i++){
    	for(int j = 0; j < x; j++){
			int bomb_count = 0;
			if(!me->tiles[i][j]->is_bomb){
				if ((i > 0 && j > 0) && me->tiles[i-1][j-1]->is_bomb){
					bomb_count++;
				}if ((j > 0) && me->tiles[i][j-1]->is_bomb){
					bomb_count++;
				}if ((i < y -1 && j > 0) && me->tiles[i+1][j-1]->is_bomb){
					bomb_count++;
				}if ((i > 0) && me->tiles[i-1][j]->is_bomb){
					bomb_count++;
				}if ((i < y -1) && me->tiles[i+1][j]->is_bomb){
					bomb_count++;
				}if ((i > 0 && j < x -1) && me->tiles[i-1][j+1]->is_bomb){
					bomb_count++;
				}if ((j < x -1) && me->tiles[i][j+1]->is_bomb){
					bomb_count++;
				}if ((i < y -1 && j < x -1) && me->tiles[i+1][j+1]->is_bomb){
					bomb_count++;
				}
				me->tiles[i][j]->num_bombs = bomb_count;
			}
		}
	}
}

static void initialize(board *me, int x, int y, int num_bombs){
	ctor_each_tile(me, x, y);
	int count = 0;
	//printf("%d\n", num_bombs);
	for(int i = 0; i < y; i++){
		for(int j = 0; j < x; j++){
			if(count >= num_bombs){
				return;
			}
			me->tiles[i][j]->p_Table->p_set_bomb(me->tiles[i][j]);
			count++;
		}
	}
}

static void add_grid_pos(GtkGrid *grid, int x, int y){
	
	GtkWidget *ebox = gtk_event_box_new();
	char *dir = malloc(strlen(directory) + strlen(images) + strlen(face) + 1 ); // Add 1 for null terminator.

	strcpy(dir, directory);
	strcat(dir, images);
	strcat(dir, face);

	GtkWidget *img = gtk_image_new_from_file(dir);
	gtk_container_add(GTK_CONTAINER(ebox), img);
	gtk_grid_attach(GTK_GRID(grid), ebox, x, y, 1, 1);

	free(dir);
}

static void reset_grid_pos(GtkGrid *grid, int x, int y){
	
	GtkWidget *ebox = gtk_grid_get_child_at(grid, x, y);
	GtkWidget *img  = gtk_bin_get_child(GTK_BIN(ebox));
	char *dir = malloc(strlen(directory) + strlen(images) + strlen(face) + 1 ); // Add 1 for null terminator.

	strcpy(dir, directory);
	strcat(dir, images);
	strcat(dir, face);

	gtk_image_set_from_file(GTK_IMAGE(img), dir);

	free(dir);
}

static void reset_grid(GtkGrid *grid, int x, int y, int new_x, int new_y){

	int min_y = (y < new_y) ? y : new_y;
	int min_x = (x < new_x) ? x : new_x;

	if(x > new_x){
		for (int i = x-1; i >= new_x; i--) {
			gtk_grid_remove_column(grid, i);
		}
	}
	
	if(y > new_y){
		for (int i = y-1; i >= new_y; i--) {
			gtk_grid_remove_row(grid, i);
		}
	}
	
	if(x < new_x){
		for (int i = x; i < new_x; i++) {
			for (int j = 0; j < min_y; j++){
				add_grid_pos(grid, i, j);
			}
		}
	}
	
	if(y < new_y){
		for (int i = y; i < new_y; i++) {
			for (int j = 0; j < min_x; j++){
				add_grid_pos(grid, j, i);
			}
		}
	}

	if(y < new_y && x < new_x ){
		for (int i = y; i < new_y; i++){
			for (int j = x; j < new_x; j++){
				add_grid_pos(grid, j, i);
			}
		}
	}

	for(int i = 0; i < min_y; i++){
		for(int j = 0; j < min_x; j++){
			reset_grid_pos(grid, j, i);
		}
	}
}

static void reset_tiles(board *me, int x, int y, int new_x, int new_y){

	int min_y = (y < new_y) ? y : new_y;
	int min_x = (x < new_x) ? x : new_x;


	if(x > new_x){
		for (int i = 0; i < min_y; i++) {
			for (int j = x-1; j >= new_x; j--) {
				free(me->tiles[i][j]->p_Table);
				free(me->tiles[i][j]);
			}
		}
	 }

	if(y > new_y){
		for (int i = y-1; i >= new_y; i--) {
			for (int j = 0; j < min_x; j++) {
				free(me->tiles[i][j]->p_Table);
				free(me->tiles[i][j]);
			}
			if(x > new_x && y > new_y){
				for (int j = x-1; j >= new_x; j--) {
					free(me->tiles[i][j]->p_Table);
					free(me->tiles[i][j]);
				}
			}
			free(me->tiles[i]);
		}
	}

	me->tiles = realloc( me->tiles, new_y * sizeof(tile **));
	for (int i = 0; i < min_y; i++){
		me->tiles[i] = realloc(me->tiles[i], new_x * sizeof(tile *));
	}

	if(x < new_x){
		for (int i = 0; i < min_y; i++) {
			for (int j = x; j < new_x; j++) {
				me->tiles[i][j] = malloc(sizeof( tile ));
				tile_ctor(me->tiles[i][j], j, i);
			}
		}
	}

	if(y < new_y){
		for (int i = y; i < new_y; i++) {
			me->tiles[i] = malloc(new_x * sizeof( tile *));
			for (int j = 0; j < min_x; j++) {
				me->tiles[i][j] = malloc(sizeof( tile ));
				tile_ctor(me->tiles[i][j], j, i);
			}		
		}
	}

	if(x < new_x && y < new_y ){
		for (int i = y; i < new_y; i++) {
		//	me->tiles[i] = malloc(new_x * sizeof( tile *));
			for (int j = x; j < new_x; j++) {
				me->tiles[i][j] = malloc(sizeof( tile ));
				tile_ctor(me->tiles[i][j], j, i);
			}	
		}
	}
}


static void new_game(board *me, int x, int y, int new_x, int new_y, int new_bombs, GtkWidget *grid){

	reset_grid(GTK_GRID(grid), x, y, new_x, new_y);;

	if(x != new_x || y != new_y){
		
		reset_tiles(me, x, y, new_x, new_y);
			
		if(x > new_x || y > new_y ) initialize(me, new_x, new_y, new_bombs);

		x = new_x;
		y = new_y;
	}
	if(me->bomb_amount != new_bombs){
		initialize(me, x, y, new_bombs);
	}

	for(int i = 0; i < y; i++){
    	for(int j = 0; j < x; j++){
			int ri = i + (random() % (y-i));
			int rj = j + (random() % (x-j));
			
			bool temp = me->tiles[i][j]->is_bomb;
        	me->tiles[i][j]->is_bomb = me->tiles[ri][rj]->is_bomb;
        	me->tiles[ri][rj]->is_bomb = temp;
		}
	}
	setup_num(me, x, y);
}

board_VTable *board_VTable_ctor(){	// swap out to struct not struct *
	board_VTable *functs = (board_VTable *)malloc(sizeof(board_VTable));
	functs->p_reveal = reveal;
	functs->p_new_game = new_game;
	functs->p_setup_num = setup_num;
	return functs;
}

board *board_ctor(board *me, int x, int y, int num){
	me = (board *)malloc(sizeof(board));
	me->tiles = (tile ***)malloc(y * sizeof(tile **));
	for (int i = 0; i < y; i++){
		me->tiles[i] = (tile **)malloc(x * sizeof(tile *));
		for (int j = 0; j < x; j++){
			me->tiles[i][j] = (tile *)malloc(sizeof(tile ));
		}
	}
	me->bomb_amount = num;
	me->end = false;
	me->p_Table = board_VTable_ctor();
	initialize(me, x, y, num);
	return me;
}
