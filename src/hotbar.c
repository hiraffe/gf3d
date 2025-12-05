#include "simple_logger.h"

#include "hotbar.h"

void hotbar_open(UI* ui)
{
	if (!ui) return;
	ui->visible = 1;
	ui->item_selected = 0;
}

void hotbar_close(UI* ui)
{
	if (!ui) return;
	ui->visible = 0;
}

void hotbar_think(UI* ui)
{
	// disappear when another menu is open

	/*
	int i;
	UI_Manager ui_manager;

	ui_manager = ui_get_manager();
	for (i = 0; i < ui_manager.ui_max; i++)
	{
		if (ui_manager.ui_list[i].visible)
		{
			ui_draw(&ui_manager.ui_list[i]);
		}
	}
	*/
}

void hotbar_draw(UI* ui)
{
	const char str[1024];
	Item* item_held;
	if (!ui) return;

	item_held = ui->mData->item_held;
	if (item_held->count == 1) {
		snprintf(str, sizeof(str), "Item held: %s", item_held->displayName);
	}
	else{
		snprintf(str, sizeof(str), "Item held: %s (%i)", item_held->displayName, item_held->count);
	}
	gf2d_font_draw_line_tag(str, FT_H2, GFC_COLOR_WHITE, gfc_vector2d(10, 650));
}

UI* hotbar_new()
{
	UI* self;
	self = ui_new();
	if (!self) return NULL;

	//populate data
	gfc_line_cpy(self->name, "Hotbar");
	gfc_line_cpy(self->type, "main");
	self->background = NULL; 
	self->item_selected = 0;
	self->item_max = 0;

	self->think = hotbar_think;
	self->draw = hotbar_draw;

	slog("hotbar created");
	return self;
}