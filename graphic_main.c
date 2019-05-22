#include "Board.h"
//#include <time.h>

#define directory getcwd(NULL, 0)

#define images "/Images/"
#define smile "Smile.svg"
#define shades "Shades.svg"

static int height, length, bomb_num;
static board *game_board;
static GtkWidget *grid;
static GtkWidget *window;
static GtkWidget *counter;
static GtkWidget *timer;
static GtkWidget *restart_ebox;
static GSource *source;

static bool is_destroyed = false;
static int milliseconds = 0;
static int unflagged_mines, selected_count;

static void btn_press_callback(GtkWidget *btn, GdkEventButton *event, gpointer user_data);
static void middle_click(GtkWidget *btn, GdkEventButton *event, gpointer user_data);

static void lose(){
	if(!is_destroyed){
		//printf("Destroy\n");
		g_source_destroy(source);
		is_destroyed = true;
	}
	game_board->p_Table->p_reveal(game_board, length, height, grid);
	for(int i = 0; i < height; i++){
	   for(int j = 0; j < length; j++){
		  GtkWidget *ebox = gtk_grid_get_child_at(GTK_GRID(grid),j,i);
		  g_signal_handlers_block_by_func(ebox ,G_CALLBACK (btn_press_callback), game_board->tiles[i][j]);
	  }
   }
}

static void win(){
	if(!is_destroyed){
		//printf("Destroy\n");
		g_source_destroy(source);
		is_destroyed = true;
	}
	for(int i = 0; i < height; i++){
	   for(int j = 0; j < length; j++){
		  GtkWidget *ebox = gtk_grid_get_child_at(GTK_GRID(grid),j,i);
		  g_signal_handlers_block_by_func(ebox, G_CALLBACK (btn_press_callback), game_board->tiles[i][j]);
	   }
   }

   char *dir = malloc(strlen(directory) + strlen(images) + strlen(shades) + 1);
   strcpy(dir, directory);
   strcat(dir, images);
   strcat(dir, shades);
   //printf("%s\n", dir);
   GtkWidget *img = gtk_bin_get_child(GTK_BIN(restart_ebox));
   gtk_image_clear(GTK_IMAGE(img));
   gtk_image_set_from_file(GTK_IMAGE(img), dir);
   gtk_widget_show(restart_ebox);
   free(dir);
}

static void flag_tile(tile *me){
	me->p_Table->p_set_flag(me, gtk_grid_get_child_at(GTK_GRID(grid), me->x_pos, me->y_pos));
}

static void select_tile(board *me, int x, int y){
	selected_count++;
	if(!me->tiles[y][x]->is_flag){
		if(me->tiles[y][x]->is_bomb){
			lose();
			return;
		}
		GtkWidget *ebox = gtk_grid_get_child_at(GTK_GRID(grid), x, y);
		g_signal_handlers_unblock_by_func(ebox ,G_CALLBACK (middle_click), me->tiles[y][x]);
		g_signal_handlers_block_by_func(ebox ,G_CALLBACK (btn_press_callback), me->tiles[y][x]);
		me->tiles[y][x]->p_Table->p_selected(me->tiles[y][x], ebox);
		me->tiles[y][x]->is_selected = true;
		if(me->tiles[y][x]->num_bombs == 0){
			for(int i = -1; i < 2; i++){
				for(int j = -1; j < 2; j++){
					if(!((y+i) < 0 || (x+j) < 0) && !((y+i) >= height || (x+j) >= length)){
						if(!me->tiles[y+i][x+j]->is_selected){
							select_tile(me, (x+j), (y+i));
						}
					}
				}
			}
		}
	}
}

static void update_counter(int num){
	char count[3];
	sprintf(count, "%d", num);
	gtk_label_set_text(GTK_LABEL(counter), count);
	gtk_widget_show(counter);
}

static void middle_click(GtkWidget *btn, GdkEventButton *event, gpointer user_data){
	tile *me = user_data;
	int x = me->x_pos;
	int y = me->y_pos;

	if (event->type == GDK_BUTTON_PRESS  &&  event->button == 2 && me->num_bombs != 0){	//2 should be middle
		int flags_around = 0;
		for(int i = -1; i < 2; i++){
			for(int j = -1; j < 2; j++){
				if(!((y+i) < 0 || (x+j) < 0) && !((y+i) >= height || (x+j) >= length)){
					if(game_board->tiles[y+i][x+j]->is_flag) flags_around++;
				}
			}
		}
		if(flags_around == me->num_bombs){
			for(int i = -1; i < 2; i++){
				for(int j = -1; j < 2; j++){
					if(!((y+i) < 0 || (x+j) < 0) && !((y+i) >= height || (x+j) >= length)){
						if(!(game_board->tiles[y+i][x+j]->is_flag || game_board->tiles[y+i][x+j]->is_selected)) {
							select_tile(game_board, (x+j), (y+i));
						}
					}
				}
			}
			GtkWidget *og_ebox = gtk_grid_get_child_at(GTK_GRID(grid), x, y);
			g_signal_handlers_block_by_func(og_ebox ,G_CALLBACK (middle_click), me);
			gtk_widget_show_all(GTK_WIDGET(window));
		}
		//printf("Middle\n");
	}
}


static void btn_press_callback(GtkWidget *btn, GdkEventButton *event, gpointer user_data){
	tile *me = user_data;
    if (event->type == GDK_BUTTON_PRESS  &&  event->button == 3){		//3 is right mouse btn
		flag_tile(me);
		if(me->is_flag) update_counter(--unflagged_mines);
		else 			update_counter(++unflagged_mines);
		//printf("Right\n");
	}
	else if(event->type == GDK_BUTTON_PRESS  &&  event->button == 1){	//1 is left mouse btn
		if(!me->is_flag){
			if(me->is_bomb){
				lose();
			}
			else {
				select_tile(game_board, me->x_pos, me->y_pos);
				GtkWidget *ebox = gtk_grid_get_child_at(GTK_GRID(grid),me->x_pos, me->y_pos);
				g_signal_handlers_block_by_func(ebox ,G_CALLBACK (btn_press_callback), me);
				//g_signal_handlers_unblock_by_func(ebox ,G_CALLBACK (middle_click), me);
			}
			//printf("Left\n");
		}
	}
	gtk_widget_show_all(GTK_WIDGET(window));
}

static void check_win(){
	if(selected_count >= (height * length - bomb_num)){
		win();
	}
}

static gboolean timer_increase(gpointer data){
	char count[4];
	GtkWidget *timer = (GtkWidget *)data;
	milliseconds = milliseconds + 100;
	sprintf(count, "%d", milliseconds/1000);
	gtk_label_set_text(GTK_LABEL(timer), count);
	gtk_widget_show(timer);
	check_win();
	return true;
}

void free_the_board(board *me){
for (int i = 0; i < height; i++) {
	for (int j = 0; j < length; j++) {
		free(me->tiles[i][j]->p_Table);
		free(me->tiles[i][j]);
	}
	free(me->tiles[i]);
}
	free(me->p_Table);
	free(me);
}

guint timeout_add_full (gint priority, guint32 interval, GSourceFunc function, gpointer data,
                        GDestroyNotify notify){
  guint id;
  g_return_val_if_fail (function != NULL, 0);

  source = g_timeout_source_new (interval);
  is_destroyed = false;

  if (priority != G_PRIORITY_DEFAULT) g_source_set_priority (source, priority);

  g_source_set_callback (source, function, data, notify);
  id = g_source_attach (source, NULL);
  g_source_unref (source);

  return id;
}

guint timeout_add (guint32 interval, GSourceFunc function, gpointer data){
  return timeout_add_full (G_PRIORITY_DEFAULT, interval, function, data, NULL);
}

static void restart(){
	milliseconds = 0;
	selected_count = 0;
	unflagged_mines = bomb_num;
	update_counter(unflagged_mines);

	if(!is_destroyed){
		//printf("Destroy\n");
		g_source_destroy(source);
		is_destroyed = true;
	}
	//printf("Timeout\n");
	timeout_add(100, timer_increase, timer);

	//printf("String\n");
	char *dir = malloc(strlen(directory) + strlen(images) + strlen(shades) + 1);
    strcpy(dir, directory);
	strcat(dir, images);
    strcat(dir, smile);

    GtkWidget *img = gtk_bin_get_child(GTK_BIN(restart_ebox));
    gtk_image_clear(GTK_IMAGE(img));
    gtk_image_set_from_file(GTK_IMAGE(img), dir);
    gtk_widget_show(restart_ebox);

	//printf("Free\n");
	free(dir);

	game_board->end = false;
	game_board->p_Table->p_new_game(game_board, length, height, grid);
	for(int i = 0; i < height; i++){
	   for(int j = 0; j < length; j++){
		   GtkWidget *ebox = gtk_grid_get_child_at(GTK_GRID(grid), j, i);
			g_signal_connect (G_OBJECT(ebox),"button-press-event",
							  G_CALLBACK (btn_press_callback), game_board->tiles[i][j]);
		  	g_signal_connect (G_OBJECT(ebox),"button-press-event",
  						  	  G_CALLBACK (middle_click), game_board->tiles[i][j]);
			g_signal_handlers_block_by_func(ebox ,G_CALLBACK (middle_click),
				  							game_board->tiles[i][j]);
		}
	}
	gtk_widget_show_all(GTK_WIDGET(window));
}

static void activate (GtkApplication* app, gpointer user_data){
  char *dir = malloc(strlen(directory) + strlen(images) + strlen(shades) + 1);

  strcpy(dir, directory);
  strcat(dir, images);
  strcat(dir, smile);

  window = gtk_application_window_new(app);
  grid = gtk_grid_new();
  counter = gtk_label_new(NULL);
  update_counter(unflagged_mines);

  timer = gtk_label_new("0");
  timeout_add(100, timer_increase, timer);

  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  GtkWidget *top_grid = gtk_grid_new();
  restart_ebox = gtk_event_box_new();
  GtkWidget *smile_img = gtk_image_new_from_file(dir);

  free(dir);

  gtk_container_add(GTK_CONTAINER(restart_ebox), smile_img);
  g_signal_connect (G_OBJECT(restart_ebox),"button-press-event",
					G_CALLBACK (restart), NULL);
  gtk_grid_attach(GTK_GRID(top_grid), timer, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(top_grid), restart_ebox, length/2, 0, 3, 1);
  gtk_grid_attach(GTK_GRID(top_grid), counter, length-1, 0, 1, 1);
  gtk_widget_set_halign(top_grid, GTK_ALIGN_CENTER);
  gtk_grid_set_column_spacing(GTK_GRID(top_grid), 10);
  game_board = board_ctor(game_board, length, height, bomb_num);
  restart();

//gtk_window_set_default_size(GTK_WINDOW(window), 500, 550);
  gtk_window_set_resizable(GTK_WINDOW(window), false);
  gtk_window_set_title(GTK_WINDOW (window), "Minesweeper");
  gtk_box_pack_start(GTK_BOX(vbox), top_grid, true, false, 5);
  gtk_box_pack_start(GTK_BOX(vbox), grid, true, false, 0);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  gtk_widget_show_all(GTK_WIDGET(window));
}

int main (int argc, char **argv){
  height = 20;
  length = 20;
  bomb_num = 70;
  selected_count = 0;
  unflagged_mines = bomb_num;
  srand(time(0));
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);

  g_object_unref (app);
  free_the_board(game_board);
  return status;
}
