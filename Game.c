#include "Game.h"

#include <string.h>

static void 
left_right_click(GtkWidget *btn, GdkEventButton *event, gpointer data);

static void 
middle_click(GtkWidget *btn, GdkEventButton *event, gpointer data);

static void 
kill_app(struct game *me);


static int
g_signal_is_blocked_by_func(gpointer widget, GFunc function, gpointer data)
{
    return g_signal_handler_find(widget, G_SIGNAL_MATCH_FUNC | 
			G_SIGNAL_MATCH_DATA | G_SIGNAL_MATCH_UNBLOCKED,
            0, 0, NULL, function, data) == 0;
}

static void 
reveal(struct game *me)
{
	struct board *gb = &me->game_board;

	for(int i = 0; i < gb->height; i++)
	{
		for(int j = 0; j < gb->width; j++)
		{

			GtkWidget *ebox = gtk_grid_get_child_at(GTK_GRID(me->grid), j, i);

			if(!g_signal_is_blocked_by_func(ebox, (GFunc) left_right_click, me))
				g_signal_handlers_block_by_func(ebox, G_CALLBACK (left_right_click), me);
		
			if(!g_signal_is_blocked_by_func(ebox, (GFunc) middle_click, me))
				g_signal_handlers_block_by_func(ebox, G_CALLBACK (middle_click), me);

			if(gb->tiles[i * gb->width + j].is_bomb || gb->tiles[i*gb->width + j].is_flag)
			{
				int len = strlen(dir) + strlen(images) + 1;

				GtkWidget *img  = gtk_bin_get_child(GTK_BIN(ebox));

				if(!gb->tiles[i*gb->width + j].is_flag && gb->tiles[i*gb->width + j].is_bomb)
				{
					len += strlen(bomb);
					char file[len];
					strcpy(file, dir);
					strcat(file, images);
					strcat(file, bomb);

					gtk_image_set_from_file(GTK_IMAGE(img), file);
				}

				else if(gb->tiles[i*gb->width + j].is_flag && !gb->tiles[i*gb->width + j].is_bomb)
				{
					len += strlen(wrong);
					char file[len];
					strcpy(file, dir);
					strcat(file, images);
					strcat(file, wrong);

					gtk_image_set_from_file(GTK_IMAGE(img), file);
				}
			}
		}
	}
}

static void 
lose(struct game *me)
{
	g_source_remove(me->source);
	reveal(me);
	me->is_paused = 1;
}

static void 
win(struct game *me)
{
	g_source_remove(me->source);
	me->is_paused = 1;

	int i, j;
	for(i = 0; i < me->game_board.height; ++i)
	{
		for(j = 0; j < me->game_board.width; ++j)
		{
			GtkWidget *ebox = gtk_grid_get_child_at(GTK_GRID(me->grid), j, i);
			
			if(!g_signal_is_blocked_by_func(ebox, (GFunc) left_right_click, me))
				g_signal_handlers_block_by_func(ebox, G_CALLBACK (left_right_click), me);
		
			if(!g_signal_is_blocked_by_func(ebox, (GFunc) middle_click, me))
				g_signal_handlers_block_by_func(ebox, G_CALLBACK (middle_click), me);
		}
	}

	char file[strlen(dir) + strlen(images) + strlen(shades) + 1];
	strcpy(file, dir);
	strcat(file, images);
	strcat(file, shades);

	GtkWidget *img = gtk_bin_get_child(GTK_BIN(me->restart_ebox));
	gtk_image_clear(GTK_IMAGE(img));
	gtk_image_set_from_file(GTK_IMAGE(img), file);
	gtk_widget_show(me->restart_ebox);

	// TODO 
	// Add pop up window to show you have won
}

static void 
flag_tile(struct game *me, struct tile *grid_tile)
{	
	if(!grid_tile->is_selected)
	{
		GtkWidget *ebox = gtk_grid_get_child_at(GTK_GRID(me->grid), grid_tile->x, grid_tile->y);
		GtkWidget *img = gtk_bin_get_child(GTK_BIN(ebox));
		gtk_image_clear(GTK_IMAGE(img));

		int addition;  
		if(grid_tile->is_flag)
		{
			char file[strlen(dir) + strlen(images) + strlen(face) + 1];
			strcpy(file, dir);
			strcat(file, images);
			strcat(file, face);

			gtk_image_set_from_file(GTK_IMAGE(img), file);
			gtk_widget_show(ebox);
			addition = 1;
		}
		else
		{
			char file[strlen(dir) + strlen(images) + strlen(flag) + 1];
			strcpy(file, dir);
			strcat(file, images);
			strcat(file, flag);

			gtk_image_set_from_file(GTK_IMAGE(img), file);
			gtk_widget_show(ebox);
			addition = -1;
		}

		if(grid_tile->is_flag)
		{
			int i, j,
				x = grid_tile->x,
				y = grid_tile->y,
				height = me->game_board.height,
				width = me->game_board.width;

			for(i = -1; i < 2; ++i)
			{
				for(j = -1; j < 2; ++j)
				{
					if(!((y+i) < 0 || (x+j) < 0) && !((y+i) >= height || (x+j) >= width))
					{
						me->game_board.tiles[(y+i) * width + x + j].adj_flags += addition;
					}
				}
			}

			toggle_flag(grid_tile);
		}
	}
}

static void
select_tile(struct game *me, struct tile *grid_tile)
{
	int x = grid_tile->x,
		y = grid_tile->y;

	me->selected_count++;
	if(!grid_tile->is_flag)
	{
		if(grid_tile->is_bomb)
		{
			lose(me);
			return;
		}

		GtkWidget *ebox = gtk_grid_get_child_at(GTK_GRID(me->grid), x, y);
		g_signal_handlers_unblock_by_func(ebox, G_CALLBACK (middle_click), me);
		g_signal_handlers_block_by_func(ebox, G_CALLBACK (left_right_click), me);

		GtkWidget *img  = gtk_bin_get_child(GTK_BIN(ebox));	
		int num_len = strlen(num_faces[grid_tile->adj_bombs]);

		char file[strlen(dir) + strlen(images) + num_len + 1];
		strcpy(file, dir);
		strcat(file, images);
		strcat(file, num_faces[grid_tile->adj_bombs]);

		gtk_image_set_from_file(GTK_IMAGE(img), file);

		grid_tile->is_selected = 1;

		if(grid_tile->adj_bombs == 0)
		{
			for(int i = -1; i < 2; i++)
			{
				for(int j = -1; j < 2; j++)
				{
					if(!((y+i) < 0 || (x+j) < 0) && !((y+i) >= me->game_board.height || (x+j) >= me->game_board.width))
					{
						struct tile *next_tile = &me->game_board.tiles[(y+i) * me->game_board.width + x+j];
						if(!next_tile->is_selected)
						{
							select_tile(me, next_tile);
						}
					}
				}
			}
		}
	}
}


static void 
update_counter(struct game *me)
{
	char count[3];	// should never excede 3 digits
	sprintf(count, "%d", me->unflagged_mines);
	gtk_label_set_text(GTK_LABEL(me->counter), count);
	gtk_widget_show(me->counter);
}


static void
get_grid_position(GtkWidget *grid, GtkWidget *ebox, int *x, int *y)
{	
	GValue value = G_VALUE_INIT;
	g_value_init(&value, G_TYPE_INT);

	gtk_container_child_get_property(GTK_CONTAINER(grid), ebox, "left-attach", &value);
	*x = g_value_get_int(&value);
	gtk_container_child_get_property(GTK_CONTAINER(grid), ebox, "top-attach", &value);
	*y = g_value_get_int(&value);
}


static void 
middle_click(GtkWidget *btn, GdkEventButton *event, gpointer data)
{
	struct game *me = data;
	int x, y,
		height = me->game_board.height, 
		width  = me->game_board.width;

	get_grid_position(me->grid, btn, &x, &y);

	struct tile *grid_tile = &me->game_board.tiles[y * width + x];

	if(event->type == GDK_BUTTON_PRESS && event->button == 2 && grid_tile->adj_bombs != 0) //2 should be middle click
	{	
		if(grid_tile->adj_flags == grid_tile->adj_bombs)
		{
			for(int i = -1; i < 2; i++)
			{
				for(int j = -1; j < 2; j++)
				{
					if(!((y+i) < 0 || (x+j) < 0) && !((y+i) >= height || (x+j) >= width))
					{
						if(!(me->game_board.tiles[(y+i) * width + x+j].is_flag ||
									me->game_board.tiles[(y+i) * width + x+j].is_selected)) 
						{
							select_tile(me, &me->game_board.tiles[(y+i) * width + x+j]);
						}
					}
				}
			}
			g_signal_handlers_block_by_func(btn, G_CALLBACK (middle_click), me);
			gtk_widget_show_all(me->grid);
		}
	}
}


static void 
left_right_click(GtkWidget *btn, GdkEventButton *event, gpointer data)
{
	struct game *me = data;
	int x, y,
		width = me->game_board.width;

	get_grid_position(me->grid, btn, &x, &y);

	struct tile *grid_tile = &me->game_board.tiles[y * width + x];

	if (event->type == GDK_BUTTON_PRESS)
	{
		if(event->button == 3) // 3 is right mouse button
		{				
			flag_tile(me, grid_tile);
			if(grid_tile->is_flag)	
				me->unflagged_mines--;
			else 			
				me->unflagged_mines++;
			update_counter(me);
		}
		else if(event->button == 1) // 1 is left mouse button
		{			
			if(!grid_tile->is_flag)
			{	
				select_tile(me, grid_tile);
			}
		}
	}
	gtk_widget_show(me->grid);
}


static void 
add_grid_pos(struct game *me, char *file, int x, int y)
{
	GtkWidget *ebox = gtk_event_box_new();
	GtkWidget *img = gtk_image_new_from_file(file);

	gtk_container_add(GTK_CONTAINER(ebox), img);

	g_signal_connect(G_OBJECT(ebox), "button-press-event", G_CALLBACK(left_right_click), me);
	g_signal_connect(G_OBJECT(ebox), "button-press-event", G_CALLBACK(middle_click), me);
	g_signal_handlers_block_by_func(ebox, G_CALLBACK (middle_click), me);

	gtk_grid_attach(GTK_GRID(me->grid), ebox, x, y, 1, 1);
}


static void 
remove_grid_pos(struct game *me, int x, int y)
{
	GtkWidget *ebox = gtk_grid_get_child_at(GTK_GRID(me->grid), x, y);
	GtkWidget *img = gtk_bin_get_child(GTK_BIN(ebox));

	g_signal_handlers_disconnect_by_func(G_OBJECT(ebox), left_right_click, me);
	g_signal_handlers_disconnect_by_func(G_OBJECT(ebox), middle_click, me);

	gtk_widget_destroy(img);
}


static void 
reset_grid_pos(struct game *me, char *file, int x, int y)
{
	GtkWidget *ebox = gtk_grid_get_child_at(GTK_GRID(me->grid), x, y);
	GtkWidget *img = gtk_bin_get_child(GTK_BIN(ebox));
	gtk_image_clear(GTK_IMAGE(img));
	gtk_image_set_from_file(GTK_IMAGE(img), file);
	
	if(g_signal_is_blocked_by_func(ebox, (GFunc) left_right_click, me))
		g_signal_handlers_unblock_by_func(ebox, G_CALLBACK (left_right_click), me);

	if(!g_signal_is_blocked_by_func(ebox, (GFunc) middle_click, me))
		g_signal_handlers_block_by_func(ebox, G_CALLBACK (middle_click), me);
}

static void 
reset_grid(struct game *me, int row, int col)
{	
	char file[strlen(dir) + strlen(images) + strlen(face) + 1]; // Add 1 for null terminator.

	strcpy(file, dir);
	strcat(file, images);
	strcat(file, face);

	int width = me->game_board.width,
		height = me->game_board.height,
		min_w = (width < width + col) ? width : width + col,
		min_h = (height < height + row) ? height : height + row;

	int i, j;	
	if(row > 0)
	{
		for(i = height; i < height + row; ++i)
		{
			for(j = 0; j < width; ++j)
			{		
				add_grid_pos(me, file, j, i);
			}
		}
	}

	else if(row < 0)
	{
		for(i = height -1; i >= height + row; --i)
		{
			for(j = 0; j < width; ++j)
			{		
				remove_grid_pos(me, j, i);
			}
			gtk_grid_remove_row(GTK_GRID(me->grid), i);
		}
	}

	if(col > 0)
	{
		for(j = width; j < width + col; ++j)
		{
			for(i = 0; i < height + row; ++i)
			{	
				add_grid_pos(me, file, j, i);
			}
		}
	}

	else if(col < 0)
	{
		for(j = width -1; j >= width + col; --j)
		{
			for(i = 0; i < height + row; ++i)
			{	
				remove_grid_pos(me, j, i);
			}
			gtk_grid_remove_column(GTK_GRID(me->grid), j);
		}
	}

	for(i = 0; i < min_h; ++i)
	{
		for(j = 0; j < min_w; ++j)
		{
			reset_grid_pos(me, file, j, i);
		}
	}
}


static gint 
timer_increase(gpointer data)
{	
	struct game *me = data;
	char count[4];

	me->milliseconds += 100;
	sprintf(count, "%d", me->milliseconds/1000);
	gtk_label_set_text(GTK_LABEL(me->timer), count);
	gtk_widget_show(me->timer);
	if(me->selected_count >= (me->game_board.height * me->game_board.width - me->game_board.b_count))
	{
		win(me);
	}
	return 1;
}

static void 
restart(struct game *me)
{
	me->milliseconds = 0;
	me->selected_count = 0;
	
	if(!me->is_paused)
	{
		g_source_remove(me->source);
	}
	me->source = g_timeout_add(100, timer_increase, me);
	me->is_paused = 0;

	int new_w = gtk_range_get_value(GTK_RANGE(me->width_slide));
	int new_h = gtk_range_get_value(GTK_RANGE(me->height_slide));
	int new_b = gtk_range_get_value(GTK_RANGE(me->bomb_slide));

	char file[strlen(dir) + strlen(images) + strlen(smile) +1];
	strcpy(file, dir);
	strcat(file, images);
	strcat(file, smile);

	GtkWidget *img = gtk_bin_get_child(GTK_BIN(me->restart_ebox));
	gtk_image_clear(GTK_IMAGE(img));
	gtk_image_set_from_file(GTK_IMAGE(img), file);
	gtk_widget_show(me->restart_ebox);

	reset_grid(me, new_h - me->game_board.height, new_w - me->game_board.width);

	if(!reset(&me->game_board, new_w, new_h, new_b))
	{
		kill_app(me);
	}

	me->unflagged_mines = new_b;
	update_counter(me);

	int visible = gtk_widget_get_visible(me->slide_vbox);
	gtk_widget_show_all(me->window);
	if(!visible)
	{
		gtk_widget_hide(me->slide_vbox);
	}
}

static void 
restart_click(GtkWidget *btn, GdkEventButton *event, gpointer data)
{
	(void) btn;
	(void) event;
	struct game *me = data;
	restart(me);
}

static void
toggle_visiblity(GtkWidget *btn, GdkEventButton *event, gpointer data)
{
	(void) btn;
	(void) event;
	GtkWidget *slide_vbox = data;
	gtk_widget_set_visible(slide_vbox, !gtk_widget_get_visible(slide_vbox));	
}

static void
disconnect_grid(struct game *me)
{
	for(int i = 0; i < me->game_board.height; i++)
	{
		for(int j = 0; j < me->game_board.width; j++)
		{
			GtkWidget *ebox = gtk_grid_get_child_at(GTK_GRID(me->grid), j, i);
			g_signal_handlers_disconnect_by_func(G_OBJECT(ebox), left_right_click, me);
			g_signal_handlers_disconnect_by_func(G_OBJECT(ebox), middle_click, me);
			gtk_widget_destroy(ebox);
		}
	}
}

static void 
kill_app(struct game *me)
{
	// Allocated Data in Struct Game
	free(me->game_board.tiles);

	// Disconnect all g_signals
	disconnect_grid(me);
	g_signal_handlers_disconnect_by_func(G_OBJECT(me->restart_ebox), restart_click, me);
	g_signal_handlers_disconnect_by_func(G_OBJECT(me->menu_ebox), toggle_visiblity, me->slide_vbox);
	g_signal_handlers_disconnect_by_func(G_OBJECT(gtk_grid_get_child_at(GTK_GRID(me->top_grid), 0, 0)), kill_app, me);
	
	if(!me->is_paused)
	{
		g_source_remove(me->source);
	}

	// Free all Widgets that are created and everything stored in them	
	gtk_widget_destroy(me->top_grid);
	gtk_widget_destroy(me->grid);
	gtk_widget_destroy(me->slide_vbox);
	//gtk_widget_destroy(gtk_bin_get_child(GTK_BIN(me->window)));

	// Close the window
	gtk_window_close(GTK_WINDOW(me->window));
}

static void 
kill_click(GtkWidget *btn, GdkEventButton *event, gpointer data)
{
	(void) btn;
	(void) event;
	struct game *me = data;
	kill_app(me);
}

void 
activate(GtkApplication* app, gpointer data)
{
	struct game *me = data;

	srandom(time(0));

	char css_file[strlen(dir) + strlen(theme) +1];
	strcpy(css_file, dir);
	strcat(css_file, theme);

	GFile *g_file = g_file_new_for_path(css_file);
	GtkCssProvider *cssProvider = gtk_css_provider_new();
	gtk_css_provider_load_from_file(cssProvider, g_file, NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
			GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	char face_file[strlen(dir) + strlen(images) + strlen(face) + 1];
	strcpy(face_file, dir);
	strcat(face_file, images);
	strcat(face_file, face);

	me->window = gtk_application_window_new(app);
	me->counter = gtk_label_new(NULL);
	me->grid = gtk_grid_new();

	int i, j;
	for(i = 0; i < me->game_board.height; ++i)
	{
		for(j = 0; j < me->game_board.width; ++j)
		{
			GtkWidget *ebox = gtk_event_box_new();
			GtkWidget *img = gtk_image_new_from_file(face_file);

			gtk_container_add(GTK_CONTAINER(ebox), img);

			g_signal_connect(G_OBJECT(ebox), "button-press-event", G_CALLBACK(left_right_click), me);
			g_signal_connect(G_OBJECT(ebox), "button-press-event", G_CALLBACK(middle_click), me);

			gtk_grid_attach(GTK_GRID(me->grid), ebox, j, i, 1, 1);
		}
	}

	me->timer = gtk_label_new("0");
	gtk_widget_set_size_request(me->timer, 35, 10);

	char file[strlen(dir) + strlen(images) + strlen(smile) + 1];
	strcpy(file, dir);
	strcat(file, images);
	strcat(file, smile);

	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	me->top_grid = gtk_grid_new();
	me->restart_ebox = gtk_event_box_new();
	GtkWidget *smile_img = gtk_image_new_from_file(file);

	GtkWidget *hbox_for_vboxes = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	me->slide_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	me->width_slide = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 10, 50, 1);
	gtk_range_set_value(GTK_RANGE(me->width_slide), me->game_board.width);
	GtkWidget *width_label = gtk_label_new("Width");

	me->height_slide = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 10, 50, 1);
	gtk_range_set_value(GTK_RANGE(me->height_slide), me->game_board.height);
	GtkWidget *height_label = gtk_label_new("Height");

	me->bomb_slide = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 10, 1000, 1);
	gtk_range_set_value(GTK_RANGE(me->bomb_slide), me->game_board.b_count);
	GtkWidget *bomb_label = gtk_label_new("Number of Mines");

	me->menu_ebox = gtk_event_box_new();
	GtkWidget *menu_label = gtk_label_new("Menu");
	GtkWidget *exit_ebox = gtk_event_box_new();
	GtkWidget *exit_label = gtk_label_new("Exit");

	gtk_grid_attach(GTK_GRID(me->top_grid), me->timer, me->game_board.width /4, 0, 2, 1);
	gtk_grid_attach(GTK_GRID(me->top_grid), me->restart_ebox, me->game_board.width /2 , 0, 2, 1);
	gtk_grid_attach(GTK_GRID(me->top_grid), me->counter, me->game_board.width *3 /4, 0, 2, 1);
	gtk_grid_attach(GTK_GRID(me->top_grid), me->menu_ebox, me->game_board.width -1, 0, 2, 1);
	gtk_grid_attach(GTK_GRID(me->top_grid), exit_ebox, 0, 0, 2, 1);
	gtk_widget_set_halign(me->top_grid, GTK_ALIGN_CENTER);
	gtk_grid_set_column_homogeneous(GTK_GRID(me->top_grid), 1);

	gtk_box_pack_start(GTK_BOX(me->slide_vbox), width_label, 1, 1, 0);
	gtk_box_pack_start(GTK_BOX(me->slide_vbox), me->width_slide, 1, 1, 0);
	gtk_box_pack_start(GTK_BOX(me->slide_vbox), height_label, 1, 1, 0);
	gtk_box_pack_start(GTK_BOX(me->slide_vbox), me->height_slide, 1, 1, 0);
	gtk_box_pack_start(GTK_BOX(me->slide_vbox), bomb_label, 1, 1, 0);
	gtk_box_pack_start(GTK_BOX(me->slide_vbox), me->bomb_slide, 1, 1, 0);

	gtk_container_add(GTK_CONTAINER(me->restart_ebox), smile_img);
	gtk_container_add(GTK_CONTAINER(me->menu_ebox), menu_label);
	gtk_container_add(GTK_CONTAINER(exit_ebox), exit_label);

	g_signal_connect (G_OBJECT(me->restart_ebox),"button-press-event",
			G_CALLBACK(restart_click), me);
	g_signal_connect (G_OBJECT(me->menu_ebox),"button-press-event",
			G_CALLBACK(toggle_visiblity), me->slide_vbox);
	g_signal_connect (G_OBJECT(exit_ebox),"button-press-event",
			G_CALLBACK(kill_click), me);

	gtk_window_set_resizable(GTK_WINDOW(me->window), 0);
	gtk_box_pack_start(GTK_BOX(vbox), me->top_grid, 1, 0, 5);
	gtk_box_pack_start(GTK_BOX(vbox), me->grid, 1, 0, 0);
	gtk_box_pack_start(GTK_BOX(hbox_for_vboxes), vbox, 1, 0, 0);
	gtk_box_pack_start(GTK_BOX(hbox_for_vboxes), me->slide_vbox, 1, 1, 20);
	gtk_container_add(GTK_CONTAINER(me->window), hbox_for_vboxes);

	gtk_widget_show(GTK_WIDGET(me->window));

	restart(me);
}


void
game_ctor(struct game *me, int w, int h, int m)
{
	board_ctor(&me->game_board, w, h, m);
	me->is_paused = 1;
	me->milliseconds = 0;
	me->selected_count = 0;
	me->unflagged_mines = m;

	// Gtk pointers can't be allocateded since the application 
	// hasn't been started. They will be allocated in activate(),
}

