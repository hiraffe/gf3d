#include "simple_logger.h"

#include "gfc_input.h"
#include "gfc_audio.h"

#include "monster.h"
#include "hotbar.h"
#include "menu.h"

#include "shop_menu.h"

Mix_Chunk* sold = NULL;

void shop_menu_open(UI* ui)
{
	ShopMenuData* data;
	if ((!ui) || (!ui->data)) return;
	data = ui->data;

	ui->item_selected = 0;
	ui->visible = 1;
	data->state = SS_Buy;

	menu_set_game_state(GS_Pause);
}

void shop_menu_close(UI* ui)
{
	if (!ui) return;
	ui->visible = 0;

	menu_set_game_state(GS_Play);
	Mix_ResumeMusic();
}

void shop_menu_think(UI* ui)
{
	ShopMenuData* data;
	if ((!ui) || (!ui->data)) return;
	data = ui->data;

	// update inventory
	inventory_update(data->shopData->sell_list);
	inventory_update(ui->mData->inventory);

	if (gfc_input_command_pressed("pandown")) {
		ui->item_selected++;
		if (ui->item_selected >= ui->item_max) {
			ui->item_selected = ui->item_max;
		}
		menu_play_selected();
	}

	if (gfc_input_command_pressed("panup")) {
		ui->item_selected--;
		if (ui->item_selected < 0) {
			ui->item_selected = 0;
		}
		menu_play_selected();
	}

	if (gfc_input_command_pressed("enter")) 
	{
		if (ui->item_selected >= ui->item_max)
		{
			ui->item_selected = 0;
			menu_play_click();
			if (data->state == SS_Buy)  {
				data->state = SS_Sell;
			}
			else {
				data->state = SS_Buy;
			}
		}
		else {
			Mix_PlayChannel(0, sold, 0);
			if (data->state == SS_Buy) {
				ui->mData->gold -= shop_sell_item(data->shopData->sell_list, ui->mData->inventory, ui->item_selected);
			}
			else {
				ui->mData->gold += shop_sell_item(ui->mData->inventory, data->shopData->sell_list, ui->item_selected);
			}
		}
	}

	if (gfc_input_command_pressed("cancel"))
	{
		menu_play_click();
		shop_menu_close(ui);
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
		if (!item->sellable) continue;

		snprintf(itemStr, sizeof(itemStr), "%s (%i)", item->displayName, item->count);
		snprintf(priceStr, sizeof(priceStr), "$%d", item->price);
		color = (i == ui->item_selected) ? GFC_COLOR_WHITE : GFC_COLOR_YELLOW;
		gf2d_font_draw_line_tag(itemStr, FT_H5, color, gfc_vector2d(505, 250+offset));
		gf2d_font_draw_line_tag(priceStr, FT_H5, color, gfc_vector2d(710, 250+offset));
		offset += 30;
	}
	color = (ui->item_max <= ui->item_selected) ? GFC_COLOR_WHITE : GFC_COLOR_YELLOW;
	gf2d_font_draw_line_tag(str, FT_H5, color, gfc_vector2d(710, 440));
}

void shop_menu_draw(UI* ui)
{
	ShopMenuData* data;
	if ((!ui)||(!ui->data)) return;
	data = ui->data;

	if (data->state == SS_Buy)
	{
		gf2d_font_draw_line_tag(ui->name, FT_H2, GFC_COLOR_YELLOW, gfc_vector2d(505, 205));
		ui->item_max = data->shopData->sell_list->itemslist->count;
		shop_draw_inventory(ui, data->shopData->sell_list, "Sell");
	}
	else if (data->state == SS_Sell)
	{
		gf2d_font_draw_line_tag("Inventory", FT_H2, GFC_COLOR_YELLOW, gfc_vector2d(560, 205));
		ui->item_max = ui->mData->inventory->itemslist->count;
		shop_draw_inventory(ui, ui->mData->inventory, "Buy");
	}

	char gold[32];
	snprintf(gold, sizeof(gold), "Total money: $%.2f", ui->mData->gold);
	gf2d_font_draw_line_tag(gold, FT_H5, GFC_COLOR_GREY, gfc_vector2d(505, 470));
}

UI* shop_menu_new(ShopEntityData* shop, const char* name)
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
	gfc_line_cpy(self->name, name);
	gfc_line_cpy(self->type, "menu");
	self->background = gf2d_sprite_load_image("images/ui/window_background.png");
	self->bg_position = gfc_vector2d(500, 200);
	self->item_max = 0;
	data->state = SS_Buy;
	data->shopData = shop;

	self->think = shop_menu_think;
	self->draw = shop_menu_draw;
	//self->free = shop_menu_free;

	sold = Mix_LoadWAV("audio/effects/coin.wav");

	slog("shop menu created: %s", self->name);
	return self;
}