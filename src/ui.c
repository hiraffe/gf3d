#include "simple_logger.h"

#include "ui.h"

static UI_Manager ui_manager = { 0 };

UI_Manager ui_get_manager()
{
	return ui_manager;
}

void ui_manager_init()
{
	ui_manager.background = gf2d_sprite_load_image("images/ui/window_background.png");
	ui_manager.visible = 0;
	slog("UI manager initialized");

	atexit(ui_manager_close);
}

void ui_manager_close()
{
	gf2d_sprite_free(ui_manager.background);
	slog("UI manager closed");
}

void ui_manager_draw()
{
	if (!ui_manager.visible) return;

	gf2d_sprite_draw_image(
		ui_manager.background,
		gfc_vector2d(500,200));

	//draw the rest of whatever needs to be here

	gf2d_mouse_draw();
}

void ui_manager_show()
{
	ui_manager.visible = 1;
}

void ui_manager_hide()
{
	ui_manager.visible = 0;
}
