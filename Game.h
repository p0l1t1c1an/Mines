#ifndef GAME_H
#define GAME_H

#include "Board.h"

#include <gtk/gtk.h>

// Path to image and css files are stored
#define dir		"/usr/local/share/Mines/"
#define images	"Images/"

// Top face state images
#define smile	"Smile.svg"
#define shades	"Shades.svg"

// Theme file for gtk
#define theme	"theme.css"

// Mine state images
#define bomb	"Mine.svg"
#define wrong	"Wrong.svg"

// Covered tile state images
#define flag	"Flag.svg"
#define face	"Face.svg"

// Numbered state images 
#define zero	"Zero.svg"
#define one		"One.svg"
#define two		"Two.svg"
#define three	"Three.svg"
#define four	"Four.svg"
#define five	"Five.svg"
#define six		"Six.svg"
#define seven	"Seven.svg"
#define eight	"Eight.svg"

static char *num_faces[9] = {zero, one, two, three, four, five, six, seven, eight};

/*
 * Game
 * Contains data on the state of the game.
 * These are the time (ms), num of mines not flags, num selected times, 
 * the if the game is over, the source id of the time, and a board. 
 * As well, there are numerous GtkWidgets. 
 */

struct game
{
	int milliseconds, unflagged_mines, 
		selected_count; 
	
	unsigned char is_paused;
	
	unsigned int source; 

	struct board game_board;
	
	GtkWidget	
		*window, *grid,

		*top_grid, *counter, *timer, 
		*restart_ebox, *menu_ebox,
				
		*slide_vbox, *width_slide,
		*height_slide, *bomb_slide;

	GdkPixbuf 	
		*num_tiles[9], *smile_tile, *shade_tile,
		*bomb_tile, *wrong_tile, *flag_tile, *face_tile; 	
};

// Starts the application and creates all GtkWidgets
void 
activate(GtkApplication* app, gpointer user_data);

// Creates a default game without creating the GtkWidgets
void
game_ctor(struct game *me, int width, int height, int bombs);

#endif //GAME_H
