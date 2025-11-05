#ifndef __SHOP_MENU__
#define __SHOP_MENU__

#include "shop.h"

typedef enum
{
	SS_Buy,
	SS_Sell
}ShopState;

typedef struct
{
	Entity*		shop;
	ShopState	state;
}ShopMenuData;

void shop_menu_open();
void shop_menu_close();
void shop_menu_think();
void shop_menu_draw();

#endif