#include "simple_logger.h"

#include "ui.h"

static UI_Manager ui_manager = { 0 };

UI_Manager ui_get_manager()
{
	return ui_manager;
}

UI* ui_new()
{
	int i;
	Entity* monster = monster_get_the();
	MonsterEntityData* data;
	if ((!monster) || (!monster->data)) return NULL;
	data = monster->data;

	if (ui_manager.ui_list)
	{
		for (i = 0; i < ui_manager.ui_max; i++)
		{
			if (!ui_manager.ui_list[i]._inuse)
			{
				//set all defaults here
				ui_manager.ui_list[i]._inuse = 1;
				ui_manager.ui_list[i].visible = 0;
				ui_manager.ui_list[i].item_selected = 0;
				ui_manager.ui_list[i].mData = data;

				return &ui_manager.ui_list[i];
			}
		}
	}
	return NULL;
}

void ui_free(UI* ui)
{
	if (!ui)return;
	if (ui->free)ui->free(ui);
	gf2d_sprite_free(ui->background);
	memset(ui, 0, sizeof(UI));
	ui->_inuse = 0;
}

void ui_manager_close()
{
	int i;
	if (ui_manager.ui_list)
	{
		for (i = 0; i < ui_manager.ui_max; i++)
		{
			ui_free(&ui_manager.ui_list[i]);
		}
		free(ui_manager.ui_list);
	}
	memset(&ui_manager, 0, sizeof(UI_Manager));
	slog("UI manager closed");
}

void ui_manager_init(Uint32 ui_max)
{
	if (!ui_max)
	{
		slog("cannot init ui system with zero ui");
		return;
	}
	ui_manager.ui_list = gfc_allocate_array(sizeof(UI), ui_max);
	if (!ui_manager.ui_list)
	{
		slog("failed to allocate %i uis for the system", ui_max);
		return;
	}
	ui_manager.ui_max = ui_max;
	atexit(ui_manager_close);

	slog("ui manager initialized");
}

void ui_draw(UI* ui)
{
	if (!ui) return;

	if (ui->background)
	{
		gf2d_sprite_draw_image(
			ui->background,
			gfc_vector2d(500, 200));
	}

	if (ui->draw)ui->draw(ui);

	//gf2d_mouse_draw();
}

void ui_manager_draw_all()
{
	int i;
	for (i = 0; i < ui_manager.ui_max; i++)
	{
		if (ui_manager.ui_list[i].visible)
		{
			ui_draw(&ui_manager.ui_list[i]);
		}
	}
}

void ui_think(UI* ui)
{
	if (!ui) return;
	if (ui->think)ui->think(ui);
}

void ui_manager_think_all()
{
	int i;
	for (i = 0; i < ui_manager.ui_max; i++)
	{
		if (ui_manager.ui_list[i].visible)
		{
			ui_think(&ui_manager.ui_list[i]);
		}
	}
}
