#include "Tile.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#define flag "Flag.svg"
#define face "Face.svg" // would be confusing to call it tile

#define directory getcwd(NULL, 0)
#define images "/Images/"

#define zero "Zero.svg"
#define one "One.svg"
#define two "Two.svg"
#define three "Three.svg"
#define four "Four.svg"
#define five "Five.svg"
#define six "Six.svg"
#define seven "Seven.svg"
#define eight "Eight.svg"

static char *num_faces[9] = {zero, one, two, three, four, five, six, seven, eight};

static void selected(tile *me, GtkWidget *ebox){
	GtkWidget *image;
	GList *children, *iter;
	children = gtk_container_get_children(GTK_CONTAINER(ebox));
	for(iter = children; iter != NULL; iter = g_list_next(iter))
  		gtk_widget_destroy(GTK_WIDGET(iter->data));
	g_list_free(children);

	char *dir = NULL;
	int dir_len = strlen(directory);
	int img_len = strlen(images);
	int num_len = strlen(num_faces[me->num_bombs]);

	dir = malloc( dir_len + img_len + num_len + 1 ); // Add 1 for null terminator.
	strcpy(dir, directory);
	strcat(dir, images);
	strcat(dir, num_faces[me->num_bombs]);
	//printf("%s\n", dir);
	image = gtk_image_new_from_file(dir);
	gtk_container_add(GTK_CONTAINER(ebox), image);
	free(dir);
	//gtk_widget_show(GTK_WIDGET(ebox));
}

static void set_bomb(tile *me){
	me->is_bomb = true;
}

static void set_flag(tile *me, GtkWidget *ebox){
	if(!me->is_selected){
		GtkWidget *image;
		GList *children, *iter;
		children = gtk_container_get_children(GTK_CONTAINER(ebox));
		for(iter = children; iter != NULL; iter = g_list_next(iter))
	  		gtk_widget_destroy(GTK_WIDGET(iter->data));
		g_list_free(children);

		char *dir = NULL;
		int dir_len = strlen(directory);
		int img_len = strlen(images);
		int flag_len = strlen(flag);
		int face_len = strlen(face);

		if(me->is_flag){
			me->is_flag = false;
			dir = malloc( dir_len + img_len + face_len + 1 ); // Add 1 for null terminator.
			strcpy(dir, directory);
			strcat(dir, images);
			strcat(dir, face);
			image = gtk_image_new_from_file(dir);
			gtk_container_add(GTK_CONTAINER(ebox), image);
		}
		else{
			me->is_flag = true;
			dir = malloc( dir_len + img_len + flag_len + 1 ); // Add 1 for null terminator.
			strcpy(dir, directory);
			strcat(dir, images);
			strcat(dir, flag);
			image = gtk_image_new_from_file(dir);
			gtk_container_add(GTK_CONTAINER(ebox), image);
		}
		free(dir);
	}

}

tile_VTable *tile_table_ctor(){
	tile_VTable *functs = (tile_VTable *)malloc(sizeof(tile_VTable));
	functs->p_selected = selected;
	functs->p_set_bomb = set_bomb;
	functs->p_set_flag = set_flag;
	return functs;
}

void tile_ctor(tile *me, int x, int y){
	me->is_bomb = false;
	me->is_flag = false;
	me->is_selected = false;
	me->x_pos = x;
	me->y_pos = y;
	me->num_bombs = 0;

	me->p_Table = tile_table_ctor();
}
