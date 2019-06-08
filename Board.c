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
			if(me->tiles[i][j]->is_bomb && me->tiles[i][j]->is_flag);

			else if(me->tiles[i][j]->is_bomb || me->tiles[i][j]->is_flag){
				char *dir = NULL;
				GtkWidget *image;
				GList *children, *iter;
				GtkWidget *ebox = gtk_grid_get_child_at(GTK_GRID(grid), j, i);

				children = gtk_container_get_children(GTK_CONTAINER(ebox));
				for(iter = children; iter != NULL; iter = g_list_next(iter))
			  						gtk_widget_destroy(GTK_WIDGET(iter->data));
				g_list_free(children);

				if(!me->tiles[i][j]->is_flag && me->tiles[i][j]->is_bomb){
					char *dir = malloc(strlen(directory) + strlen(images) + strlen(bomb) + 1 ); // Add 1 for null terminator.
					strcpy(dir, directory);
					strcat(dir, images);
					strcat(dir, bomb);
					//printf("%s\n", "NF, YB" );
					image = gtk_image_new_from_file(dir);
				}

				else if(me->tiles[i][j]->is_flag && !me->tiles[i][j]->is_bomb){
					char *dir = malloc(strlen(directory) + strlen(images) + strlen(wrong_flag) + 1 ); // Add 1 for null terminator.
					strcpy(dir, directory);
					strcat(dir, images);
					strcat(dir, wrong_flag);
					image = gtk_image_new_from_file(dir);
				}

				gtk_container_add(GTK_CONTAINER(ebox), image);
				free(dir);
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
				}if ((i < y -1 && j < y -1) && me->tiles[i+1][j+1]->is_bomb){
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
				goto HERE;
			}
			me->tiles[i][j]->p_Table->p_set_bomb(me->tiles[i][j]);
			count++;
		}
	}
	HERE:
	setup_num(me, x, y);
}

static void reset_grid_pos(int x, int y, GtkWidget *grid){
		GtkWidget *ebox = gtk_event_box_new();
		GtkWidget *image;
		char *dir = malloc(strlen(directory) + strlen(images) + strlen(face) + 1 ); // Add 1 for null terminator.

		strcpy(dir, directory);
		strcat(dir, images);
		strcat(dir, face);
		image = gtk_image_new_from_file(dir);
		gtk_container_add(GTK_CONTAINER(ebox), image);
		gtk_grid_attach(GTK_GRID(grid), ebox, x, y, 1, 1);

		free(dir);
}

static void reset_tiles(board *me, int x, int y, int new_x, int new_y, int new_bombs){
	for (int i = 0; i < y; i++) {
		for (int j = 0; j < x; j++) {
			free(me->tiles[i][j]->p_Table);
			free(me->tiles[i][j]);
		}
		free(me->tiles[i]);
	}
	free(me->tiles);

	me->tiles = (tile ***)malloc(new_y * sizeof(tile **));
	for (int i = 0; i < new_y; i++){
		me->tiles[i] = (tile **)malloc(new_x * sizeof(tile *));
		for (int j = 0; j < new_x; j++){
			me->tiles[i][j] = (tile *)malloc(sizeof(tile ));
		}
	}

	initialize(me, new_x, new_y, new_bombs);

}

static void new_game(board *me, int x, int y, int new_x, int new_y, int new_bombs, GtkWidget *grid){

	GList *children, *iter;

	children = gtk_container_get_children(GTK_CONTAINER(grid));
	for(iter = children; iter != NULL; iter = g_list_next(iter))
						gtk_widget_destroy(GTK_WIDGET(iter->data));
	g_list_free(children);

	if(x != new_x || y != new_y || me->bomb_amount != new_bombs){
		reset_tiles(me, x, y, new_x, new_y, new_bombs);
		x = new_x;
		y = new_y;
	}

	for(int i = 0; i < y; i++){
    	for(int j = 0; j < x; j++){
			int ri = i + (random() % (y-i));
			int rj = j + (random() % (x-j));
			me->tiles[i][j]->is_flag = false;
			me->tiles[i][j]->is_selected = false;
			bool temp = me->tiles[i][j]->is_bomb;
        	me->tiles[i][j]->is_bomb = me->tiles[ri][rj]->is_bomb;
        	me->tiles[ri][rj]->is_bomb = temp;
			reset_grid_pos(j, i, grid);
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
