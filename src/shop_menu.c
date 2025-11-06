#include "simple_logger.h"

#include "ui.h"
#include "shop.h"
#include "monster.h"

#include "shop_menu.h"

void shop_menu_open(UI* ui)
{
	if (!ui) return;
	ui->visible = 1;
}

void shop_menu_close()
{
	ui_manager_hide();
}

void shop_menu_think(UI* ui)
{
	UI_Manager ui_manager = ui_get_manager();
	const Uint8* keystate = SDL_GetKeyboardState(NULL);
	ShopMenuData* data;
	if ((!ui) || (!ui->data)) return;
	data = ui->data;

	if (keystate[SDL_SCANCODE_DOWN]) {
		ui->item_selected++;
		if (ui->item_selected >= ui->item_max) {
			ui->item_selected = ui->item_max;
		}
	}

	if (keystate[SDL_SCANCODE_UP]) {
		ui->item_selected--;
		if (ui->item_selected <= 0) {
			ui->item_selected = 0;
		}
	}

	if (keystate[SDL_SCANCODE_SELECT]) {
		if (ui->item_selected >= ui->item_max)
		{
			if (data->state == SS_Buy) {
				data->state = SS_Sell;
			}
			else {
				data->state = SS_Buy;
			}
		}
		else {
			shop_sell_item(data->shopData->sell_list, ui->mData->inventory, ui->item_selected);
		}
	}

	if (keystate[SDL_SCANCODE_ESCAPE])
	{
		shop_menu_close();
	}
}

void shop_draw_inventory(UI* ui, Inventory* inv, char str[16])
{
	Item* item;
	GFC_Color color;
	char itemStr[32];
	char priceStr[32];
	int offset = 0;
	if ((!inv)||(!ui)) return;

	for (int i = 0; i < inv->itemslist->count; i++)
	{
		item = gfc_list_get_nth(inv->itemslist, i);
		if (item->count <= 0) continue;

		snprintf(itemStr, sizeof(itemStr), "%s (%i)", item->name, item->count);
		snprintf(priceStr, sizeof(priceStr), "$%d", item->price);
		color = (i == ui->item_selected) ? GFC_COLOR_WHITE : GFC_COLOR_YELLOW;
		gf2d_font_draw_line_tag(itemStr, FT_H5, color, gfc_vector2d(505, 250+offset));
		gf2d_font_draw_line_tag(priceStr, FT_H5, color, gfc_vector2d(720, 250+offset));
		offset += 30;
	}
	color = (ui->item_max-1 <= ui->item_selected) ? GFC_COLOR_WHITE : GFC_COLOR_YELLOW;
	gf2d_font_draw_line_tag(str, FT_H5, color, gfc_vector2d(710, 440));
}

void shop_menu_draw(UI* ui)
{
	UI_Manager ui_manager = ui_get_manager();
	ShopMenuData* data;
	if ((!ui)||(!ui->data)) return;
	data = ui->data;

	if (data->state == SS_Buy)
	{
		gf2d_font_draw_line_tag("Seed Shop", FT_H2, GFC_COLOR_YELLOW, gfc_vector2d(560, 205));
		ui->item_max = data->shopData->sell_list->itemslist->count+1;
		shop_draw_inventory(ui, data->shopData->sell_list, "Sell");
	}
	else if (data->state == SS_Buy)
	{
		gf2d_font_draw_line_tag("Inventory", FT_H2, GFC_COLOR_YELLOW, gfc_vector2d(560, 205));
		ui->item_max = ui->mData->inventory->itemslist->count + 1;
		shop_draw_inventory(ui, ui->mData->inventory, "Buy");
	}

	char gold[32];
	snprintf(gold, sizeof(gold), "Total money: $%.2f", ui->mData->gold);
	gf2d_font_draw_line_tag(gold, FT_H5, GFC_COLOR_GREY, gfc_vector2d(505, 410));
}

UI* shop_menu_new(ShopEntityData* shop)
{
	UI* self;
	ShopMenuData* data;
	self = ui_new();
	if (!self) return NULL;
	data = gfc_allocate_array(sizeof(ShopMenuData), 1);
	if (!data)
	{
		free(self);
		return NULL;
	}
	self->data = data;

	//populate data
	gfc_line_cpy(self->name, "Seed Shop");
	self->background = gf2d_sprite_load_image("images/ui/window_background.png");
	self->item_selected = 0;
	self->item_max = 0;
	data->state = SS_Buy;
	data->shopData = shop;

	self->think = shop_menu_think;
	self->draw = shop_menu_draw;
	//self->free = shop_menu_free;

	slog("shop menu created: %s", self->name);
	return self;
}