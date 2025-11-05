#include "simple_logger.h"

#include "ui.h"
#include "shop.h"
#include "monster.h"

#include "shop_menu.h"

static int item_selected = 0;
static int item_max = 0;
ShopState state = SS_Buy;

void shop_menu_open()
{
	ui_manager_show();
	item_selected = 0;
}

void shop_menu_close()
{
	ui_manager_hide();
}

void shop_menu_think()
{
	Entity* monster = monster_get_the();
	UI_Manager ui_manager = ui_get_manager();
	const Uint8* keystate = SDL_GetKeyboardState(NULL);

	if (!ui_manager.visible) return;

	if (keystate[SDL_SCANCODE_DOWN]) {
		item_selected++;
		if (item_selected >= item_max) {
			item_selected = item_max;
		}
	}
	if (keystate[SDL_SCANCODE_UP]) {
		item_selected--;
		if (item_selected <= 0) {
			item_selected = 0;
		}
	}

	if (keystate[SDL_SCANCODE_ESCAPE])
	{
		shop_menu_close();
	}
}

void shop_draw_inventory(Inventory* inv)
{
	Item* item;
	GFC_Color color;
	char str[128];
	int offset = 0;
	if (!inv)return;

	for (int i = 0; i < inv->itemslist->count; i++)
	{
		item = gfc_list_get_nth(inv->itemslist, i);
		if (item->count <= 0) continue;

		color = (i == item_selected) ? GFC_COLOR_WHITE : GFC_COLOR_YELLOW;
		gf2d_font_draw_line_tag(item->name, FT_H5, color, gfc_vector2d(505, 250+offset));
		//display the price SOMEHOW
		offset += 30;
	}
	color = (item_max-2 == item_selected) ? GFC_COLOR_WHITE : GFC_COLOR_YELLOW;
	gf2d_font_draw_line_tag("Buy", FT_H5, color, gfc_vector2d(505, 420));
	color = (item_max-1 <= item_selected) ? GFC_COLOR_WHITE : GFC_COLOR_YELLOW;
	gf2d_font_draw_line_tag("Sell", FT_H5, color, gfc_vector2d(705, 420));

}

void shop_menu_draw()
{
	UI_Manager ui_manager = ui_get_manager();
	Entity* monster = monster_get_the();
	Entity* shop = shop_get_the();
	MonsterEntityData* mData;
	ShopEntityData* data;
	if ((!monster)||(!shop)||(!shop->data)) return;
	mData = monster->data;
	data = shop->data;

	if (!ui_manager.visible) return;

	ui_manager_draw();

	gf2d_font_draw_line_tag("Seed Shop", FT_H2, GFC_COLOR_YELLOW, gfc_vector2d(560, 205));

	if (state == SS_Buy) {
		item_max = data->sell_list->itemslist->count+2;
		shop_draw_inventory(data->sell_list);
	}
	else if (state == SS_Sell) {
		item_max = mData->inventory->itemslist->count+2;
		shop_draw_inventory(mData->inventory);
	}
}