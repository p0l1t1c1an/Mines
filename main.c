#include "Game.h"

#define DEFAULT_WIDTH	10
#define DEFAULT_HEIGHT	10
#define DEFAULT_MINES	20

int 
main(int argc, char **argv)
{
	GtkApplication *app;
	int status;

	struct game g;
	game_ctor(&g, DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_MINES);

	app = gtk_application_new ("mines.p0l1t1c1an", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), &g);
	status = g_application_run (G_APPLICATION(app), argc, argv);
	
	g_object_unref(app);

	return status;
}
