#ifndef __SHOP_MENU__
#define __SHOP_MENU__

#include "shop.h"

#include "ui.h"

typedef enum
{
	SS_Buy,
	SS_Sell
}ShopState;

typedef struct
{
	ShopState		state;
	ShopEntityData* shopData;
}ShopMenuData;

UI* shop_menu_new(ShopEntityData* shop, const char* name);

void shop_menu_open(UI* ui);

void shop_menu_close(UI* ui);

void shop_menu_think(UI* ui);

void shop_menu_draw(UI* ui);

#endif